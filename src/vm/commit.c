#include "../../include/vm/common.h"
#include "../../include/vm/commit.h"
#include "../../include/logging/logger.h"
#include "../../include/vm/hashmap.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <stdarg.h>


int add_changes(struct repository *repo, struct file_node* files) {

    // step1: check if track file is exist -> if not create it
    char track_dir_path[MAX_PATH];
    strcpy(track_dir_path, repo->dir);
    strcat(track_dir_path, "/.vsync");
    // check if .vsync exists
    DIR* dir = opendir(track_dir_path);
    if (dir == NULL)
    {
        logger(ERROR_TAG, "[add_changes] Can't find .vsync");
        return FAIL;
    }
    closedir(dir);

    strcat(track_dir_path, "/tracked"); 
    // check if tracked dir exists
    dir = opendir(track_dir_path);
    if (dir == NULL)
    {
        if (mkdir(track_dir_path, 0777) != 0)
        {
            logger(ERROR_TAG, "Can not create a tracking dir");
            return FAIL;
        }
    }
    closedir(dir);

    // check if track file exist + load infos + update changes
    struct hash_map* map = (struct hash_map*) malloc(sizeof(struct hash_map));
    init_hash_map(map);

    char track_file_path[MAX_PATH];
    strcpy(track_file_path, track_dir_path);
    strcat(track_file_path, "/track");
    if ((access(track_file_path, F_OK) == 0) && (populate_hashmap_from_file(map, track_dir_path, track_file_path) == FAIL)){
        return FAIL;
    }

    // traversal the changes and old changes for removing duplicated ones
    struct file_node* trav= files;
    while (trav != NULL) {
        // check if files existed
        if (access(trav->path, F_OK) != 0) {
            logger(ERROR_TAG, "Can not adding changes that not existed... check the added files");
            clear_hash_map(map);
            return FAIL;
        }
        
        // check if a tracked changes is existed for this file
        char* tr = get_hash_from_path(map, trav->path);

        // remove it
        if (tr != NULL) {

            char old_file_path[MAX_PATH];
            strcpy(old_file_path, track_dir_path);
            strcat(old_file_path, "/");
            strcat(old_file_path, tr);

            
            if (remove(old_file_path) != 0) {
                logger(ERROR_TAG, "[add_changes] Can not adding changes");
                return FAIL;
            }

            delete_from_map(map, trav->path);
        }
        
        // generate the hash
        char hash_string[HASH_LEN];  // +1 for null terminator
        create_hash(hash_string);
        insert_map(map, trav->path, hash_string, trav->path);

        trav = trav->next;
    }

    // add this changes
    if (make_changes(repo, map) == FAIL) {
        logger(ERROR_TAG, "[add_changes] Can not persist changes");
        return FAIL;
    }


    clear_hash_map(map);
    return SUCCESS;
}

int make_changes(struct repository* repo, struct hash_map* map) {
    char track_file_path[MAX_PATH];
    char track_dir_path[MAX_PATH];
    strcpy(track_dir_path, repo->dir);
    strcat(track_dir_path, "/.vsync/tracked");
    strcat(track_file_path, track_dir_path);
    strcat(track_file_path, "/track");

    FILE* track_file = fopen(track_file_path, "w");

    if (track_file == NULL) {
        logger(ERROR_TAG, "[make_changes] Can not open track file");
        return FAIL;
    }

    // write the first letter
    fprintf(track_file, "-\n");
    fprintf(track_file, "-\n");

    for (int i = 0; i < TABLE_SIZE; i++)
    {
        struct key_value* current = map->table[i];

        while (current != NULL){  
            char destination_path[MAX_PATH];
            strcpy(destination_path, track_dir_path);
            strcat(destination_path, "/");
            strcat(destination_path, current->hash);

            if (strcmp(current->raw_path, destination_path) != 0) {
                FILE* changes_file = fopen(current->raw_path, "rb");
                FILE* destination_file = fopen(destination_path, "wb");

                if (changes_file == NULL) {
                    custom_printf("raw: %s \n", current->raw_path);
                    logger(ERROR_TAG, "[make_changes] Can not open changes file");
                    return FAIL;            
                }
                if (destination_file == NULL) {
                    logger(ERROR_TAG, "[make_changes] Can not open destination file");
                    return FAIL;            
                }

                // copying from source to destination
                char buffer[1024];
                size_t bytesRead;
                while ((bytesRead = fread(buffer, 1, sizeof(buffer), changes_file)) > 0) {
                    fwrite(buffer, 1, bytesRead, destination_file);
                }

                fclose(changes_file);
                fclose(destination_file);
            }

            fprintf(track_file, "%s %s\n", current->path, current->hash);
            current = current->next;
        }
        
    }

    fclose(track_file);
    return SUCCESS;
}

int make_init_map_repo(struct repository* repo, struct hash_map* map, const char* path) {
    DIR *dir = opendir(path);
    struct dirent *entry;

    // Check if the directory can be opened
    if (dir == NULL) {
        perror("[make_init_map_repo] Can not open the init commit dir");
        return FAIL;
    }


    // Read each entry in the directory
    while ((entry = readdir(dir)) != NULL) {
        // Ignore "." and ".." directories
        if (strcmp(entry->d_name, ".") != 0 
            && strcmp(entry->d_name, "..") != 0
            && strcmp(entry->d_name, ".vsync") != 0) {

            // Construct the full path of the entry
            char fullpath[PATH_MAX];
            snprintf(fullpath, sizeof(fullpath), "%s/%s", path, entry->d_name);

            // If the entry is a directory, recursively call the function
            if (entry->d_type == DT_DIR) {
                make_init_map_repo(repo, map, fullpath);
            } else {
                // generate the hash
                char hash_string[HASH_LEN];  // +1 for null terminator
                create_hash(hash_string);

                insert_map(map, fullpath, hash_string, fullpath);
            }
        }
    }
    
    return SUCCESS;
}


int make_commit(struct repository* repo, struct commit* commit) {
    // step0: make the hash of the commit and link it with last commit
    // constructing the hash
    create_hash(commit->hash);

    // mapping the commit hash
    strcpy(commit->parent_hash, repo->last_commit->hash);
    // step1: read changes file and the last commit files in hashmap
    char track_path[MAX_PATH];
    strcpy(track_path, repo->dir);
    strcat(track_path, "/.vsync/tracked");
    
    DIR* tracked_dir = opendir(track_path);
    if (tracked_dir == NULL)
    {
        logger(ERROR_TAG, "Can not make a commit without changes");
        return FAIL;
    }

    closedir(tracked_dir);

    strcat(track_path, "/track");
    if (access(track_path, F_OK) != 0)
    {
        logger(ERROR_TAG, "Can not make a commit without changes");
        return FAIL;
    }


    struct hash_map* file_changes_map = (struct hash_map*) malloc(sizeof(struct hash_map));
    struct hash_map* last_commit = (struct hash_map*) malloc(sizeof(struct hash_map));

    init_hash_map(file_changes_map);
    init_hash_map(last_commit);

    char last_commit_info_path[MAX_PATH];
    strcpy(last_commit_info_path, repo->dir);
    strcat(last_commit_info_path, "/.vsync/");
    strcat(last_commit_info_path, commit->parent_hash);
    strcat(last_commit_info_path, "/commit");
    // populate_hashmap_from_file(file_changes_map, track_path);
    // populate_hashmap_from_file(last_commit, last_commit_info_path);
    /*
        step2: traversal the last commit when comparing the changes
        for creating the new commit
        at end delete the changes files
    */


   // make the commit dir and open the info file
   char new_commit_dir_path[MAX_PATH];
   strcpy(new_commit_dir_path, repo->dir);
   strcat(new_commit_dir_path, "/.vsync/");
   strcat(new_commit_dir_path, commit->hash);

    if (mkdir(new_commit_dir_path, 0777) != 0)
    {
        logger(ERROR_TAG, "Can not create the commit dir");
        return FAIL;
    }
    
    char new_commit_info_path[MAX_PATH];
    strcpy(new_commit_info_path,new_commit_dir_path);
    strcat(new_commit_info_path, "/commit");
    FILE* new_commit_info_file = fopen(new_commit_info_path, "w");
    if (new_commit_info_file == NULL)
    {
        logger(ERROR_TAG, "Can not create the info file for the new commit");
        return FAIL;
    }
    
    fprintf(new_commit_info_file, "%s\n", commit->parent_hash);
    
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        struct key_value* current = last_commit->table[i];

        while (current != NULL)
        {
            // copying from last commit to the new commit

            // the source file
            char source_path[MAX_PATH];
            strcpy(source_path, repo->dir);
            FILE* source_file;

            char *hash =  get_hash_from_path(file_changes_map, current->hash);
                
            if (hash != NULL) {
                strcat(source_path, "/.vsync/tracked/");
                strcat(source_path, hash);
            } else {
                strcat(source_path, "/.vsync/");
                strcat(source_path, commit->parent_hash);
                strcat(source_path, "/");
                strcat(source_path, current->hash);
                hash = current->hash;
            }
            

            source_file = fopen(source_path, "rb");
            if (source_file == NULL)
            {
                logger(ERROR_TAG, "Can not open the source for copying");
                fclose(new_commit_info_file);
                return FAIL;
            }
            

            // the destinantion file
            char destination_path[MAX_PATH];
            strcpy(destination_path, new_commit_dir_path);
            strcat(destination_path, "/");
            strcat(destination_path, hash);

            
            FILE* destination_file = fopen(destination_path, "wb");
            if (destination_file == NULL)
            {
                logger(ERROR_TAG, "Can not open the destination file for writing");
                fclose(source_file);
                fclose(new_commit_info_file);
                return FAIL;
            }  

            
            // copying from source to destination
            char buffer[1024];
            size_t bytesRead;
            while ((bytesRead = fread(buffer, 1, sizeof(buffer), source_file)) > 0) {
                buffer[bytesRead] = 0;
                fwrite(buffer, 1, bytesRead, destination_file);
            }

            // copying infos to new commmit info file
            fprintf(new_commit_info_file, "%s %s\n", current->path, hash);
            
            fclose(destination_file);
            fclose(source_file);

            current = current->next;
        }
        
    }
    
    // add the other changes
    for (int i = 0; i < TABLE_SIZE; i++)
    {
        struct key_value* current = file_changes_map->table[i];     
        while (current != NULL) {
        
            char* hash = get_hash_from_path(last_commit, current->path);

            if (hash == NULL)
            {
                
                char source_path[MAX_PATH];
                strcpy(source_path, repo->dir);
                strcat(source_path, "/.vsync/tracked/");
                strcat(source_path, current->hash);
                FILE* source_file = fopen(source_path, "rb");
                if (source_file == NULL) {
                    logger(ERROR_TAG, "Can not open the source for copying");
                    fclose(new_commit_info_file);
                    return FAIL;
                }
                

                char destination_path[MAX_PATH];
                strcpy(destination_path, repo->dir);
                strcat(destination_path, "/.vsync/");
                strcat(destination_path, commit->hash);
                strcat(destination_path, current->hash);
                FILE* destination_file = fopen(destination_path, "wb");
                if (destination_file == NULL) {
                    logger(ERROR_TAG, "Can not open the destination file for writing");
                    fclose(source_file);
                    fclose(new_commit_info_file);
                    return FAIL;
                }  


                // copying from source to destination
                char buffer[1024];
                size_t bytesRead;
                while ((bytesRead = fread(buffer, 1, sizeof(buffer), source_file)) > 0) {
                    fwrite(buffer, 1, bytesRead, destination_file);
                }

                // copying infos to new commmit info file
                fprintf(new_commit_info_file, "%s %s", current->path, hash);

                fclose(source_file);
                fclose(destination_file);
            }
            
            current = current->next;
        }
    }

    fclose(new_commit_info_file);
    clear_hash_map(last_commit);
    clear_hash_map(file_changes_map);
    
    // add the last commit info to repository file

    return SUCCESS;
}


int create_commit(struct repository* repo, struct commit* commit, struct hash_map* map) {
    // create the commit dir
    char commit_path[MAX_PATH];
    strcpy(commit_path, repo->dir);
    strcat(commit_path, "/.vsync/");
    strcat(commit_path, commit->hash);

    if (mkdir(commit_path, 0777) != 0) {
        logger(ERROR_TAG, "[create_commit] Can not create the commit dir");
        return FAIL;
    }
    
    // create the commit file and copy the raw the data
    strcat(commit_path, "/commit");

    FILE* commit_file = fopen(commit_path, "w");

    if (commit_file == NULL) {
        logger(ERROR_TAG, "[create_commit] Can not create the commit file");
        return FAIL;
    }

    // write the basics info
    fprintf(commit_file, "%s\n", commit->parent_hash);
    fprintf(commit_file, "%s %s\n", commit->author->username, commit->author->mail);

    // make commit path = ...../.vsync/hash/
    commit_path[strlen(commit_path) - 6] = 0;
    
    for(int i = 0; i < TABLE_SIZE; i++) {
        struct key_value* current = map->table[i];
        
        while (current != NULL)
        {
            // open the src file
            FILE* source_file = fopen(current->raw_path, "rb");
            if (source_file == NULL)
            {
                logger(ERROR_TAG, "[create_commit] can not open the src file");
                return FAIL;
            }
            

            // open the dest file
            char destination_path[MAX_PATH];
            strcpy(destination_path, commit_path);
            strcat(destination_path, current->hash);

            FILE* destination_file = fopen(destination_path, "wb");
            if (destination_file == NULL)
            {
                logger(ERROR_TAG, "[create_commit] can not open the destination file");
                return FAIL;
            }
            

            // copying from source to destination
            char buffer[1024];
            size_t bytesRead;
            while ((bytesRead = fread(buffer, 1, sizeof(buffer), source_file)) > 0) {
                fwrite(buffer, 1, bytesRead, destination_file);
            }

            fclose(source_file);
            fclose(destination_file);

            fprintf(commit_file, "%s %s\n", current->path, current->hash);

            current = current->next;
        }
    }


    fclose(commit_file);

    return SUCCESS;
}

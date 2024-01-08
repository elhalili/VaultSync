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

int add_files(struct repository *repo, struct file_node* files) {
    // step1: check if track file is exist -> if not create it
    char track_dir[strlen(repo->dir) + 23];
    strcpy(track_dir, repo->dir);
    strcat(track_dir, "/.vsync");
    // check if .vsync exists
    DIR* dir = opendir(track_dir);
    if (dir == NULL)
    {
        logger(ERROR_TAG, "Can't find .vsync");
        return FAIL;
    }
    closedir(dir);

    strcat(track_dir, "/tracked"); 
    // check if tracked exists
    dir = opendir(track_dir);
    if (dir == NULL)
    {
        if (mkdir(track_dir, 0777) != 0)
        {
            logger(ERROR_TAG, "Can not create a tracking dir");
            return FAIL;
        }
    }
    closedir(dir);
    
    strcat(track_dir, "/track");
    struct hash_map* map = (struct hash_map*) malloc(sizeof(struct hash_map));
    for (size_t i = 0; i < TABLE_SIZE; i++)
    {
        map->table[i] = NULL;
    }
    
    // make a hashmap of tracked files
    if ((access(track_dir, F_OK) == 0) && populate_hashmap_from_file(map, track_dir) == FAIL) {
        return FAIL;
    }

    // step2: check if files already exists -> if exist update them
    struct file_node* trav = files;

    // FILE* track_file = fopen(track_dir, "w");

    // if (track_file == NULL)
    // {
    //     logger(ERROR_TAG, "Can not write changes");
    //     return FAIL;
    // }

    // make track_dir only on .../.vsync/tracked/

    track_dir[strlen(track_dir) - 5] = '\0';
    

    while (trav != NULL) {
        // check if a tracked changes is existed for this file
        char* tr = get_value_from_key(map, trav->path);

        // remove it
        if (tr != NULL) {

            char file_path[strlen(track_dir) + HASH_LEN];
            strcpy(file_path, track_dir);
            strcat(file_path, tr);

            
            if (remove(file_path) != 0) {

                logger(ERROR_TAG, "Can not adding changes");
                return FAIL;
            }

            delete_from_map(map, trav->path);
        }
        
        // generate the hash
        char hash_string[2 * SHA256_DIGEST_LENGTH + 1];  // +1 for null terminator
        generate_hash(hash_string);
        insert_map(map, trav->path, hash_string);

        trav = trav->next;
    }
    

    // step3: creating copies in .../.vsync/tracked/ and file the track file
    // Traverse the hashmap
    char track_file_path[strlen(repo->dir) + 23];
    strcpy(track_file_path, repo->dir);
    strcat(track_file_path, "/.vsync/tracked/track");
    FILE* track_File = fopen(track_file_path, "w");
    if (track_File == NULL) 
    {
        logger(ERROR_TAG, "Can not persist changes");
        return FAIL;
    }
    

    for (int i = 0; i < TABLE_SIZE; i++) {
        struct key_value* current = map->table[i];
        while (current != NULL) {

            // Open the source file for reading
            FILE* source_file = fopen(current->key, "rb");
            if (source_file == NULL) {
                logger(ERROR_TAG, "Can not copying changes");
                return FAIL;
            }

            // Open the destination file for writing
            char destination_path[strlen(track_dir) + HASH_LEN];
            strcpy(destination_path, track_dir);
            strcat(destination_path, current->value);
            FILE* destination_file = fopen(destination_path, "wb");
            if (destination_file == NULL) {
                logger(ERROR_TAG, "Error opening destination file");
                fclose(source_file);
                fclose(track_File);
                return FAIL;
            }

            // Copy the content of the source file to the destination file
            char buffer[1024];
            size_t bytesRead;
            while ((bytesRead = fread(buffer, 1, sizeof(buffer), source_file)) > 0) {
                fwrite(buffer, 1, bytesRead, destination_file);
            }

            // Add this to track file
            char track_buffer[strlen(current->key) + strlen(current->value) + 1];
            strcpy(track_buffer, current->key);
            strcat(track_buffer, " ");
            strcat(track_buffer, current->value);
            strcat(track_buffer, "\n");
            fprintf(track_File, track_buffer);

            // Close the files
            fclose(source_file);
            fclose(destination_file);

            current = current->next; // Move to the next entry in case of collisions
        }
    }

    fclose(track_File);
    clear_hash_map(map);

    return SUCCESS;
}

int init_commit(struct repository* repo, struct commit* commit, struct hash_map* map, const char* path) {
    DIR *dir;
    struct dirent *entry;

    // init commit directory
    char init_commit_dir[MAX_PATH];
    strcpy(init_commit_dir, repo->dir);
    strcat(init_commit_dir, "/.vsync/0/");

    // Open the directory
    dir = opendir(path);

    // Check if the directory can be opened
    if (dir == NULL) {
        perror("Error opening directory");
        exit(EXIT_FAILURE);
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
            
            // generate the hash
            char hash_string[2 * SHA256_DIGEST_LENGTH + 1];  // +1 for null terminator
            generate_hash(hash_string);
            
            // Open the source file for reading
            FILE* source_file = fopen(fullpath, "rb");
            if (source_file == NULL) {
                logger(ERROR_TAG, "Can not copying the first commit");
                return FAIL;
            }

            // Open the destination file for writing
            char destination_path[strlen(init_commit_dir) + HASH_LEN];
            strcpy(destination_path, init_commit_dir);
            strcat(destination_path, hash_string);
            FILE* destination_file = fopen(destination_path, "wb");
            if (destination_file == NULL) {
                logger(ERROR_TAG, "Error opening destination file");
                fclose(source_file);
                return FAIL;
            }

            // Copy the content of the source file to the destination file
            char buffer[1024];
            size_t bytesRead;
            while ((bytesRead = fread(buffer, 1, sizeof(buffer), source_file)) > 0) {
                fwrite(buffer, 1, bytesRead, destination_file);
            }


            insert_map(map, fullpath, hash_string);
            // Close the files
            fclose(source_file);
            fclose(destination_file);
            // If the entry is a directory, recursively call the function
            if (entry->d_type == DT_DIR) {
                init_commit(repo, commit, map, fullpath);
            }
        }
    }

    if (repo->last_commit != NULL) free(repo->last_commit);
    
    strcpy(commit->parent_hash, "-");
    strcpy(commit->hash, "0");
    repo->last_commit = commit;
    // Close the directory
    closedir(dir);

    return SUCCESS;
}

void custom_printf(const char *format, ...) {
    char buffer[1024];  // Adjust the buffer size as needed
    va_list args;
    va_start(args, format);

    // Use vsnprintf to format the string and store it in the buffer
    int len = vsnprintf(buffer, sizeof(buffer), format, args);

    // Check if vsnprintf was successful
    if (len >= 0 && len < sizeof(buffer)) {
        // Use write to print to stdout
        write(STDOUT_FILENO, buffer, len);
    }

    va_end(args);
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
    populate_hashmap_from_file(file_changes_map, track_path);
    populate_hashmap_from_file(last_commit, last_commit_info_path);
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

            char *hash =  get_value_from_key(file_changes_map, current->key);
                
            if (hash != NULL) {
                strcat(source_path, "/.vsync/tracked/");
                strcat(source_path, hash);
            } else {
                strcat(source_path, "/.vsync/");
                strcat(source_path, commit->parent_hash);
                strcat(source_path, "/");
                strcat(source_path, current->value);
                hash = current->value;
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
            fprintf(new_commit_info_file, "%s %s\n", current->key, hash);
            
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
        
            custom_printf("%p", current);
            char* hash = get_value_from_key(last_commit, current->key);

            if (hash == NULL)
            {
                
                char source_path[MAX_PATH];
                strcpy(source_path, repo->dir);
                strcat(source_path, "/.vsync/tracked/");
                strcat(source_path, current->value);
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
                strcat(destination_path, current->value);
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
                fprintf(new_commit_info_file, "%s %s", current->key, hash);

                fclose(source_file);
                fclose(destination_file);
            }
            
            current = current->next;
        }
    }

    fclose(new_commit_info_file);
    clear_hash_map(last_commit);
    clear_hash_map(file_changes_map);

    return SUCCESS;
}

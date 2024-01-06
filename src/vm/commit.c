#include "../../include/common.h"
#include "../../include/vm/commit.h"
#include "../../include/logging/logger.h"
#include "../../include/vm/hashmap.h"
#include "../../include/crypto/hashing.h"
#include "../crypto/hashing.c"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>

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
        
        // Set a seed for the random number generator
        srand(time(NULL));

        char random_data[RANDOM_SIZE];
        generate_random_data(random_data, RANDOM_SIZE); 
        // Generate hash from random data
        char hash[HASH_LEN];
        generate_hash(random_data, HASH_LEN, hash);
        char hash_string[2 * SHA256_DIGEST_LENGTH + 1];  // +1 for null terminator
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
            sprintf(&hash_string[2 * i], "%02x", hash[i]);
        }
        hash_string[HASH_LEN] = 0;
        insert_map(map, trav->path, hash_string);

        trav = trav->next;
    }
    

    // step3: creating copies in .../.vsync/tracked/ and file the track file
    // Traverse the hashmap
    char track_file_path[strlen(repo->dir) + 23];
    strcpy(track_file_path, repo->dir);
    strcat(track_file_path, "/.vsync/tracked/track");
    FILE* track_File = fopen(track_file_path, "w");
    printf("track : %s", track_file_path);
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
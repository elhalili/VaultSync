#include "../../include/common.h"
#include "../../include/vm/commit.h"
#include "../../include/logging/logger.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <dirent.h>


int extract_tracked_files(char** files, const char* track_path) {
      
    FILE *track_file = fopen(track_path, "r");
    if (track_file == NULL) return FAIL;

    char line[MAX_LINE_LENGTH];
    char hash[HASH_LEN];
    char path[MAX_PATH_LENGTH];
    size_t filesCount = 0; 

    while (fgets(line, sizeof(line), track_file) != NULL) {

        if (sscanf(line, "%16s %s", hash, path) != 2) {
            fclose(track_file);
            return FAIL;
        } 

        char *newPair = malloc(strlen(path) + 1);
        if (newPair == NULL) {
            fclose(track_file);
            return FAIL;
        }

        strcpy(newPair, path);
        
        files[filesCount] = newPair;
        filesCount++;
    }

    // TODO: if the file is exist but empty 
    files[filesCount] = NULL;
    fclose(track_file);
    return SUCCESS; 
}

int get_status_untracked_files(char* cwd, char** tracked_files) {
    DIR *dir = opendir(cwd);
    if (dir == NULL) return FAIL;

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR) {
            // Skip "." and ".." directories
            if (!strcmp(entry->d_name, ".")
            && !strcmp(entry->d_name, "..") 
            && !strcmp(entry->d_name, ".vsync")) {
                char path[1024];
                snprintf(path, sizeof(path), "%s/%s", cwd, entry->d_name);
                get_status_untracked_files(path, tracked_files);  // Recursive call for subdirectories
            }
        } 
        else {
            // check if the file is tracked and if has been changed
            if (is_contains(tracked_files, entry->d_name))
            {
                // step1: get the hash
                char track_file_path[strlen(cwd) + 16];
                strcat(track_file_path, cwd);
                strcat(track_file_path, "/.vsync/track");
                FILE* track_file = fopen(track_file_path, "r");
                if (track_file == NULL) return FAIL;

                char line[MAX_LINE_LENGTH];
                char hash[HASH_LEN];
                char path[MAX_PATH_LENGTH];

                while (fgets(line, sizeof(line), track_file) != NULL) {

                    if (sscanf(line, "%15s %511s", hash, path) != 2) {
                        fclose(track_file);
                        // TODO: handle memory leak
                        return FAIL;
                    } 
                    
                }
                // step2: read the file
                // step3: read the actual file
                // compare files
                // print the result
            }
            printf("Tracked: " KRED " %s " KNORMAL, entry->d_name);
            printf("%s/%s\n", cwd, entry->d_name);
        }
    }
}
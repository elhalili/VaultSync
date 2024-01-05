#include "../include/common.h"
#include <stdio.h>
#include <dirent.h>
#include <string.h>

int create_file(const char* filename, const char* content) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) return FAIL;
    if (fprintf(file, content) < 0) return FAIL;

    fclose(file);

    return SUCCESS;
}

int is_dir_exists(const char *parentDir, const char *targetDir) {

    DIR *dir = opendir(parentDir);
    if (dir == NULL) return FAIL;
    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (entry->d_type == DT_DIR && strcmp(entry->d_name, targetDir) == 0) {
            closedir(dir);
            return 1; 
        }
    }

    closedir(dir);
    return 0;
}


int is_contains(char** arr, char* str) {
    while(*arr != NULL) {
        if (strcmp(*arr, str)) return 1;
    }

    return 0;
}
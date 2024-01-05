#ifndef COMMON_H
#define COMMON_H

#define SUCCESS 1
#define FAIL -1

int create_file(const char* filename, const char* content);
int is_dir_exists(const char *parentDir, const char *targetDir);
int is_contains(char** arr, char* str);

#endif
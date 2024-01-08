#ifndef COMMON_H
#define COMMON_H

#define SUCCESS 1
#define FAIL -1
#include <openssl/sha.h>

#define HASH_LEN 2 * SHA256_DIGEST_LENGTH + 1

int create_file(const char* filename, const char* content);
int is_dir_exists(const char *parentDir, const char *targetDir);
int is_contains(char** arr, char* str);
void make_string(char* dest, const char* src, ...); 
void create_hash(char* str);
#endif

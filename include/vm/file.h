#ifndef FILE_H
#define FILE_H
#include "./commit.h"
#include <openssl/sha.h>

#define MAX_PATH 128
#define HASH_LEN 2 * SHA256_DIGEST_LENGTH + 1


struct file_node {
    char path[MAX_PATH];
    char hash[HASH_LEN];
    struct file_node* next;
};


struct file_node* addFile(struct file_node* head, const char* path, const char* hash);
struct file_node* removeFileByHash(struct file_node* head, const char* hash);
struct file_node* removeFileByPath(struct file_node* head, const char* path);
struct file_node* findByHash(struct file_node* head, const char* hash);
struct file_node* findByPath(struct file_node* head, const char* path);
#endif
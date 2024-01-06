#ifndef COMMIT_H
#define COMMIT_H
#include "author.h"
#include "file.h"
#include "repository.h"
#include <openssl/sha.h>

#define HASH_LEN 2 * SHA256_DIGEST_LENGTH + 1
#define TRACKED_DIR "tracked"


struct commit {
    char* hash;
    struct author author;
    char* parent_hash;
    struct file_node* files;
};

// TODO: show differences, commits history... and all related stuffs
/**
 * Initilize the commit
*/
void init_commit(struct commit* commit);

/**
 * Track files for a commit
*/
int add_files(struct repository *repo, struct file_node* files);

/**
 * Persist the commit and preparing for the next one
*/
void add_commit(struct commit* commit);

#endif
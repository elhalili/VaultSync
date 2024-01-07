#ifndef COMMIT_H
#define COMMIT_H
#include "author.h"
#include "file.h"
#include "hashmap.h"
#include "repository.h"
#include <openssl/sha.h>

#define HASH_LEN 2 * SHA256_DIGEST_LENGTH + 1
#define TRACKED_DIR "tracked"


struct commit {
    char* hash;
    struct author author;
    char* parent_hash;
};

// TODO: show differences, commits history... and all related stuffs
/**
 * create the first commit
*/
int init_commit(struct repository* repo, struct commit* commit, struct hash_map* map, const char* path);
/**
 * Track files for a commit
*/
int add_files(struct repository *repo, struct file_node* files);

/**
 * Persist the commit and preparing for the next one
*/
void add_commit(struct commit* commit);

#endif
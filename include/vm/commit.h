#ifndef COMMIT_H
#define COMMIT_H
#include "author.h"
#include "file.h"
#include "hashmap.h"
#include "repository.h"

#define HASH_LEN 2 * SHA256_DIGEST_LENGTH + 1
#define TRACKED_DIR "tracked"


struct commit {
    char hash[HASH_LEN];
    struct author* author;
    char parent_hash[HASH_LEN];
};

// TODO: show differences, commits history... and all related stuffs
/**
 * create the map for initial commmit
*/
int make_init_map_repo(struct repository* repo, struct hash_map* map, const char* path);

/**
 * Track files for a commit
*/
int add_changes(struct repository *repo, struct file_node* files);
int make_changes(struct repository* repo, struct hash_map* map);
/**
 * Persist the commit
*/
int make_commit(struct repository* repo, struct commit* commit);

/**
 * creating a commit
 * you give a repository and commit object and a hashmap and it create the 
 * the commit dir and copying all data depending on the hashmap
*/
int create_commit(struct repository* repo, struct commit* commit, struct hash_map* map);

/**
 * Give a full repository object and a an empty commit and it will create the commit
*/
int foo(struct repository* repo, struct author* author, struct commit* commit);
/**
 * Delete the tracked directory
*/
int delete_tracked_dir(struct repository* repo);
#endif

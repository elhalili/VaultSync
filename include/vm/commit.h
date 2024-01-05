#ifndef COMMIT_H
#define COMMIT_H
#include "author.h"

#define HASH_LEN 32
#define TRACKED_DIR "tracked"
// TODO: be careful
#define MAX_LINE_LENGTH 1024
#define MAX_PATH_LENGTH 512
#define MAX_TRACKED_FILES 128


struct commit {
    char* hash;
    struct author author;
    char* parent_hash;
    char** files;
};

// TODO: show differences, commits history... and all related stuffs
/**
 * Initilize the commit
*/
void init_commit(struct commit* commit);
/**
 * Track files for a commit
*/
void add_files(char** files);
/**
 * Persist the commit and preparing for the next one
*/
void add_commit(struct commit* commit);
/**
 * Extract tracked filenames 
*/
int extract_tracked_files(char** files, const char* track_path);
/**
 * status of untracked files
*/
int get_status_untracked_files(char* cwd, char** tracked_files);
#endif
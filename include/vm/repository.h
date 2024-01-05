#ifndef REPOSITORY_H
#define REPOSITORY_H

#define CONFIG_FILE "/config"
#define REPOSITORY_FILE "/repository"
#define LOG_FILE "/logs"

#include "author.h"

// TODO : add remote related info
struct repository {
    struct author* author;
    char* dir;
    struct commit* last_commit;
};

/**
 * create the initial content when initializing
*/
char* init_repo_content(struct repository* repo);
/**
 * creating a repository object
*/
struct repository* create_repo();
/**
 * responsable for doing the initilization stuffs
*/
int init_repo(struct repository* repo);
/**
 * show status of tracked and untracked files in a given moment
*/
int get_status(struct repository* repo);

#endif
#ifndef REPOSITORY_H
#define REPOSITORY_H

#define CONFIG_FILE "/config"
#define REPOSITORY_FILE "/repository"
#define LOG_FILE "/logs"

// TODO : 
#define REPOSITORY_FILE_INIT "cwd \n" \
    "   the last commit \n" \
    "other meta-data"

#include "author.h"

// TODO : add remote related info
struct repository {
    struct author* author;
    char* dir;
    struct commit* last_commit;
};

struct repository* create_repo();
int init_repo(struct repository* repo);
int get_status(struct repository* repo);

#endif
#include "../../include/vm/repository.h"
#include "../../include/logging/logger.h"
#include "../../include/common.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>

struct repository* create_repo() {
    struct repository* repo = (struct repository*)malloc(sizeof(struct repository));

    repo->author = NULL;
    repo->dir = NULL;
    repo->last_commit  = NULL;

    return repo;
}

int init_repo(struct repository* repo) {
    // TODO: ensure the buffer size is appropriate (size = enough size + "/.vaultsync"), it can lead to many problems
    // TODO: check the existence of an old setup
    char *cwd = (char*) malloc(4096); 
    if (getcwd(cwd, 4096) == NULL) return FAIL;
    repo->dir = cwd;
    char *tmp = (char*) malloc(strlen(cwd) + 9);
    strcpy(tmp, cwd);
    char* dotdir = strcat(tmp, "/.vsync");
    if (mkdir(dotdir, 0777) != 0) return FAIL;


    // setup files: repository + config + logs
    char* config_file = malloc(strlen(dotdir) + strlen(CONFIG_FILE) + 1);
    char* repository_file = malloc(strlen(dotdir) + strlen(REPOSITORY_FILE) + 1);
    char* log_file = malloc(strlen(dotdir) + strlen(LOG_FILE) + 1);

    
    strcat(strcat(repository_file, dotdir), REPOSITORY_FILE);
    strcat(strcat(config_file, dotdir), CONFIG_FILE);
    strcat(strcat(log_file, dotdir), LOG_FILE);

    if (create_file(repository_file, REPOSITORY_FILE_INIT) == FAIL) return FAIL;
    // TODO: init config handling
    if (create_file(config_file, "") == FAIL) return FAIL;
    if (create_file(log_file, "") == FAIL) return FAIL;


    free(config_file);
    free(repository_file);
    free(log_file);
    free(cwd);
    free(dotdir);

    return SUCCESS;
}
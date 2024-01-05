#include "../../include/vm/repository.h"
#include "../../include/common.h"
#include "../../include/vm/commit.h"
#include "../../include/logging/logger.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>

char* init_repo_content(struct repository* repo) {
    int dirlen = strlen(repo->dir);
    char* content = malloc(dirlen + 4);
    strcat(content, repo->dir);
    strcat(content, "\n 0");

    return content;
}


struct repository* create_repo() {
    struct repository* repo = (struct repository*)malloc(sizeof(struct repository));

    repo->author = NULL;
    repo->dir = NULL;
    repo->last_commit  = NULL;

    return repo;
}

int init_repo(struct repository* repo) {
    // TODO: ensure the buffer size is appropriate (size = enough size + "/.vsync"), it can lead to many problems
    char *cwd = (char*) malloc(4096); 
    if (getcwd(cwd, 4096) == NULL) return FAIL;
    repo->dir = cwd;
    char *tmp = (char*) malloc(strlen(cwd) + 9);
    char* dotdir = strcat(strcpy(tmp, cwd), "/.vsync");
    // TODO: check the existence of an old setup
    if (mkdir(dotdir, 0777) != 0) return FAIL;


    // setup files: repository + config + logs
    char* config_file = malloc(strlen(dotdir) + strlen(CONFIG_FILE) + 1);
    char* repository_file = malloc(strlen(dotdir) + strlen(REPOSITORY_FILE) + 1);
    char* log_file = malloc(strlen(dotdir) + strlen(LOG_FILE) + 1);

    
    strcat(strcpy(repository_file, dotdir), REPOSITORY_FILE);
    strcat(strcpy(config_file, dotdir), CONFIG_FILE);
    strcat(strcpy(log_file, dotdir), LOG_FILE);

    char* init_rep_content = init_repo_content(repo);
    if (create_file(repository_file, init_rep_content) == FAIL) return FAIL;
    // TODO: init config handling
    if (create_file(config_file, "") == FAIL) return FAIL;
    if (create_file(log_file, "") == FAIL) return FAIL;


    free(config_file);
    free(repository_file);
    free(log_file);
    free(dotdir);
    free(init_rep_content);

    return SUCCESS;
}

int get_status(struct repository* repo) {
    // step0: check if there is a ./vsync dir
        
    if (!is_dir_exists(repo->dir, ".vsync")) {
        return FAIL;
    }
    
    // step1: check if there are tracked files in ./vsync
    char* dotdir = malloc(strlen(repo->dir) + 9);
    strcpy(dotdir, repo->dir);
    strcat(dotdir, "/.vsync");

    char** tracked_files = malloc(sizeof(char*)*50);
    int dir_exists = is_dir_exists(dotdir, TRACKED_DIR);

    if (dir_exists == FAIL) {
        free(dotdir);
        return FAIL;
    }

    // Check the track file is existed
    char* track_file_path = malloc(strlen(dotdir) + 18);
    strcpy(track_file_path, dotdir);
    strcat(track_file_path, "/tracked/track");
    if (dir_exists 
        && (access(track_file_path, F_OK) == 0) 
        && extract_tracked_files(tracked_files, track_file_path) == FAIL
    ) {
        free(dotdir);
        free(track_file_path);
        return FAIL;
    }


    // print the tracked files    
    char** tr_files_trav = tracked_files;
    while (*tr_files_trav != NULL)
    {
        printf("Tracked: " KGREEN " %s \n" KNORMAL, *tr_files_trav);

        tr_files_trav++;
    }
    // TODO: this must done
    // step2: scan all dirs and subdirectories & check if the cwd files that is tracked is changed
    // if (get_status_untracked_files(repo->dir, tracked_files) == FAIL) {
    //     free(dotdir);
    //     free(track_file_path);
    //     return FAIL;
    // }


    
    // final step: free memory
    
    return SUCCESS;
}
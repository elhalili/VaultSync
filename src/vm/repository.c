#include "../../include/vm/repository.h"
#include "../../include/common.h"
#include "../../include/vm/commit.h"
#include "../../include/logging/logger.h"
#include "../../include/vm/author.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>

void init_repo_content(struct repository* repo, char *buffer) {
    strcpy(buffer, repo->dir);
    strcat(buffer, "\n0");
}


struct repository* create_repo() {
    struct repository* repo = (struct repository*)malloc(sizeof(struct repository));

    repo->author = NULL;
    repo->last_commit  = NULL;

    return repo;
}

int init_repo(struct repository* repo) {
    // TODO: ensure the buffer size is appropriate (size = enough size + "/.vsync"), it can lead to many problems
    char cwd[MAX_CWD]; 
    if (getcwd(cwd, MAX_CWD) == NULL) {
        logger(ERROR_TAG, "Can not initialize the project");
        return FAIL;
    };

    strcpy(repo->dir, cwd);
    char dotdir[strlen(cwd) + 9];
    strcat(strcpy(dotdir, cwd), "/.vsync");
    // TODO: check the existence of an old setup
    if (mkdir(dotdir, 0777) != 0) {
        logger(ERROR_TAG, "Can not create .vsync dir");
        return FAIL;
    }


    // setup files: repository + config + logs
    char config_file[strlen(dotdir) + strlen(CONFIG_FILE) + 1];
    char repository_file[strlen(dotdir) + strlen(REPOSITORY_FILE) + 1];
    char log_file[strlen(dotdir) + strlen(LOG_FILE) + 1];

    
    strcat(strcpy(repository_file, dotdir), REPOSITORY_FILE);
    strcat(strcpy(config_file, dotdir), CONFIG_FILE);
    strcat(strcpy(log_file, dotdir), LOG_FILE);

    char init_rep_content[MAX_REPOSITORY_FILE_INIT];
    init_repo_content(repo, init_rep_content);
    if (create_file(repository_file, init_rep_content) == FAIL) return FAIL;
    // TODO: init config handling
    if (create_file(config_file, "") == FAIL) return FAIL;
    if (create_file(log_file, "") == FAIL) return FAIL;

    // last step creating the initial commit

    // make the first commit
    struct commit* commit = (struct commit*) malloc(sizeof(struct commit));
    commit->hash = "0";
    commit->parent_hash = "-";
    struct hash_map* map = (struct hash_map*)malloc(sizeof(struct hash_map));

    // craeting the commit dir
    char init_commit_dir[MAX_PATH];
    strcpy(init_commit_dir, repo->dir);
    strcat(init_commit_dir, "/.vsync/0");

    // making the init commit directory
    if (mkdir(init_commit_dir, 0777) != 0) {
        logger(ERROR_TAG, "Can not create the initial commit dir");
        return FAIL;
    }

    // initiale commit made
    init_commit(repo, commit, map, repo->dir);

    // saving info about the commit
    char commit_info_path[MAX_CWD];
    strcpy(commit_info_path, init_commit_dir);
    strcat(commit_info_path, "/commit");

    FILE* commit_info_file = fopen(commit_info_path, "w");
    fprintf(commit_info_file, "-\n");

    for (int i = 0; i < TABLE_SIZE; i++) {
        struct key_value* current = map->table[i];
        while (current != NULL) {

            // Add this to track file
            char track_buffer[strlen(current->key) + strlen(current->value) + 2];
            strcpy(track_buffer, current->key);
            strcat(track_buffer, " ");
            strcat(track_buffer, current->value);
            strcat(track_buffer, "\n");
            fprintf(commit_info_file, track_buffer);

            current = current->next; // Move to the next entry in case of collisions
        }
    }

    fclose(commit_info_file);

    return SUCCESS;
}

int get_status(struct repository* repo) {
    return SUCCESS;
}


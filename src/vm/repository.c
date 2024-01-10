#include "../../include/vm/repository.h"
#include "../../include/vm/common.h"
#include "../../include/vm/commit.h"
#include "../../include/logging/logger.h"
#include "../../include/vm/author.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <dirent.h>

int init_repo(struct repository* repo, struct author* author) {

    // TODO: ensure the buffer size is appropriate (size = enough size + "/.vsync"), it can lead to many problems
    repo = (struct repository*) malloc(sizeof(struct repository));

    strcpy(repo->name, "foo");

    if (getcwd(repo->dir, MAX_PATH) == NULL) {
        logger(ERROR_TAG, "[init_repo] Can not get the current working directory");
        return FAIL;
    };
    
    repo->author = author;

    struct commit* first_commit = (struct commit*) malloc(sizeof(struct commit));
    strcpy(first_commit->hash, "0");
    stpcpy(first_commit->parent_hash, "-");
    first_commit->author = author;

    repo->last_commit = first_commit;
    


    char dotdir_path[MAX_PATH];
    strcpy(dotdir_path, repo->dir);
    strcat(dotdir_path, "/.vsync");

    // TODO: check the existence of an old setup
    if (mkdir(dotdir_path, 0777) != 0) {
        logger(ERROR_TAG, "[init_repo] Can not create .vsync dir");
        return FAIL;
    }
    
    if (write_repository_file(repo) == FAIL)
    {
        logger(ERROR_TAG, "[init_repo] Can not create repository file");
        return FAIL;
    }
    
    // make the first commit
    struct hash_map* map = (struct hash_map*)malloc(sizeof(struct hash_map));
    init_hash_map(map);

    // TODO: refactor this (merge the code that copies files and the info commit file)
    // in
    if (make_init_map_repo(repo, map, repo->dir) == FAIL) {
        logger(ERROR_TAG, "[init_repo] can not create the hashmap for the first commit");
        return FAIL;
    }
    
    if (create_commit(repo, first_commit, map) == FAIL)
    {
        logger(ERROR_TAG, "[init_repo] can not create the first commit");
        return FAIL;
    }
    
    // free resources
    clear_hash_map(map);
    return SUCCESS;
}


int write_repository_file(struct repository* repo) {
    char repo_file_path[MAX_PATH];
    strcpy(repo_file_path, repo->dir);
    strcat(repo_file_path, "/.vsync/repository");

    FILE* repo_file = fopen(repo_file_path, "w");
    if (repo_file == NULL)
    {
        logger(ERROR_TAG, " [write_repository_file] Can not open the repository file");
        return FAIL;
    }

    fprintf(repo_file, "%s\n", repo->dir);
    fprintf(repo_file, "%s\n", repo->last_commit->hash);
    fprintf(repo_file, "%s\n", repo->name);
    fprintf(repo_file, "%s %s", repo->author->username, repo->author->mail);

    fclose(repo_file);

    return SUCCESS;
}

int load_reposotory(struct repository* repo) {
    // start looking for .vsync from the cwd, if not start recursively
    return SUCCESS;
}
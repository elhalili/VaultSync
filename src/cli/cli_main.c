#include "../../include/cli/cli_main.h"
#include "../../include/logging/logger.h"
#include "../../include/vm/author.h"
#include "../../include/vm/repository.h"
#include "../../include/vm/common.h"
#include "../../include/vm/commit.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
/**
 * The code that is wrapped by comment like thats "----" should be replaced by the right code
*/

/**
 * the cli
*/
void run_cli(int argc, char const *argv[]) {
    if (argc < 2) {
        logger(INFO_TAG, "Invalid arguments, check --help");
        return;
    } 
    else if (argc < 3) {
        if (strcmp(argv[1], ARG_INIT) == 0) {
            struct author* author = NULL;
            if ((author = load_author()) == NULL) {
                logger(ERROR_TAG, "Can not getting the author, check the default config at ~/.vsync_rc");
                return;
            }
            
            struct repository* repo = NULL;
            if (init_repo(repo, author) == SUCCESS) {
                logger(INFO_TAG, "Initialization done");
                return;
            }     

            return;       
        } else if (strcmp(argv[1], ARG_COMMIT) == 0) {
            struct commit* new_commit = (struct commit*) malloc(sizeof(struct commit));
            struct repository* repo = NULL;
            struct author* author;

            if ((author = load_author()) == NULL) {
                logger(ERROR_TAG, "Can not getting the author, check the default config at ~/.vsync_rc");
                return;
            }

            if ((repo = load_repository()) == NULL) {
                logger(ERROR_TAG, "Can not getting the repo info");
                return;            
            }

            if (foo(repo, author, new_commit) == SUCCESS) {
                logger(INFO_TAG, "The commit has been done successfully");
            } 

            return;
        } else if (strcmp(argv[1], ARG_HELP) == 0 || strcmp(argv[1], ARG_HELP_SC) == 0) {
            getting_help();
            return;
        }
        else {
            invalid_args();
            return;   
        }
    } else if ((argc < 4) && strcmp(argv[1], ARG_ROLLBACK) == 0) {
        struct repository* repo = NULL;
        if ((repo = load_repository()) == NULL) {
            logger(ERROR_TAG, "Can not getting the repo info");
            return;            
        }

        if (rollback(repo, argv[2]) == SUCCESS) {
            logger(INFO_TAG, "The rollback has been successfully done");
        }

        return;
    }
    
    if (strcmp(argv[1], ARG_ADD_CHANGES) == 0) {
        // TODO: check if files empty
        int i = 2;
        struct file_node* files = NULL;
        while (i < argc) {
            files = addFile(files, argv[i++]);
        }

        // TODO: control the entered file before consume them
        struct repository* repo = NULL;
        if ((repo = load_repository()) == NULL) {
            logger(ERROR_TAG, "Can not getting the repo info");
            return;            
        }

        if (add_changes(repo, files) == SUCCESS) {
            logger(INFO_TAG, "Files has been tracked successfully");
        }

        return; 
    }

    invalid_args();
}

void getting_help() {
    printf("The help guide: \n");
    printf("%s,%s: for getting help\n", ARG_HELP_SC, ARG_HELP);
    printf("%s: initialize a repository\n", ARG_INIT);
    printf("%s: for made a commit\n", ARG_COMMIT);
    printf("%s [files]: add files to be tracked in the next commit\n", ARG_ADD_CHANGES);
    printf("End\n");
}

void invalid_args() {
    logger(INFO_TAG, "Invalid arguments, check --help");
}
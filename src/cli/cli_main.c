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
    
    if (argc < 3) {
        if (strcmp(argv[1], ARG_INIT) == 0) {
            // TODO: look for default config to add the author
            // suppose that we have the author
            // ------------------------------------------------------
            struct author* author = (struct author*) malloc(sizeof(struct author));
            strcpy(author->username, "john");
            stpcpy(author->mail, "john@example.com");
            // ------------------------------------------------------

            struct repository* repo = NULL;

            if (init_repo(repo, author) == SUCCESS) {
                logger(INFO_TAG, "Initialization done");
                return;
            }     

            return;       
        } else if (strcmp(argv[1], ARG_COMMIT) == 0) {
            struct commit* new_commit = (struct commit*) malloc(sizeof(struct commit));
            struct repository* repo = NULL;
            struct author* author = NULL;
            /** get the repository object + the author + creating a new commit
             * TODO: implement -> load_reposotory(repo); the next code is just a replacement for the real one
            */
            // ------------------------------------------------------
            // author of commit + repository
            author = (struct author*) malloc(sizeof(struct author));
            strcpy(author->username, "john");
            stpcpy(author->mail, "john@example.com");
            
            // last commit info
            struct commit* last_commit = (struct commit*) malloc(sizeof(struct commit));
            last_commit->author = author;
            strcpy(last_commit->hash, "0");
            strcpy(last_commit->parent_hash, "-");
            
            // repo infos
            repo = (struct repository*) malloc(sizeof(struct repository));
            repo->author = author;
            repo->last_commit = last_commit;
            strcpy(repo->name, "foo_name_Repo");
            if (getcwd(repo->dir, MAX_PATH) == NULL) {
                logger(ERROR_TAG, "[CLI] Can not get the current working directory");
                return FAIL;
            };
            // ------------------------------------------------------

            if (foo(repo, author, new_commit) == SUCCESS)
            {
                logger(INFO_TAG, "The commit has been done successfully");
            } 

            return;
        } else if (strcmp(argv[1], ARG_HELP) == 0 
            || strcmp(argv[1], ARG_HELP_SC) == 0) {
            printf("The help guide: \n");
            printf("%s,%s: for getting help\n", ARG_HELP_SC, ARG_HELP);
            printf("%s: initialize a repository\n", ARG_INIT);
            printf("%s: for made a commit\n", ARG_COMMIT);
            printf("%s [files]: add files to be tracked in the next commit\n", ARG_ADD_CHANGES);
            printf("End\n");

            return;
        }
        else {
            logger(INFO_TAG, "Invalid arguments, check --help");
            return;   
        }
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
        /** get the repository object + the author + creating a new commit
         * TODO: implement -> load_reposotory(repo); the next code is just a replacement for the real one
        */
        // ------------------------------------------------------
        // author of commit + repository
        struct author* author = NULL;
        author = (struct author*) malloc(sizeof(struct author));
        strcpy(author->username, "john");
        stpcpy(author->mail, "john@example.com");
        
        // last commit info
        struct commit* last_commit = (struct commit*) malloc(sizeof(struct commit));
        last_commit->author = author;
        strcpy(last_commit->hash, "0");
        strcpy(last_commit->parent_hash, "-");
        
        // repo infos
        repo = (struct repository*) malloc(sizeof(struct repository));
        repo->author = author;
        repo->last_commit = last_commit;
        strcpy(repo->name, "foo_name_Repo");
        if (getcwd(repo->dir, MAX_PATH) == NULL) {
            logger(ERROR_TAG, "[CLI] Can not get the current working directory");
            return;
        };
        // ---------------------------------------------------------

        if (add_changes(repo, files) == SUCCESS) {
            logger(INFO_TAG, "Files has been tracked successfully");
        }

        return; 
    }

    logger(INFO_TAG, "Invalid arguments, check --help");
}
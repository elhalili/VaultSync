#include "../../include/cli/cli_main.h"
#include "../../include/cli/cli_vm.h"
#include "../../include/logging/logger.h"
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
        invalid_args();
        return;
    } 
    else if (argc < 3) {
        int init_action = !strcmp(argv[1], ARG_INIT);
        int commit_action = !strcmp(argv[1], ARG_COMMIT);
        int help_action = !strcmp(argv[1], ARG_HELP) || !strcmp(argv[1], ARG_HELP_SC);

        if (init_action) init_repository();
        else if (commit_action) commit_changes();
        else if (help_action) getting_help();   
        else invalid_args();

        return;
    } else if ((argc < 4) && strcmp(argv[1], ARG_ROLLBACK) == 0) {
        rollback_changes(argv[2]);
        return;
    }
    
    if (strcmp(argv[1], ARG_ADD_CHANGES) == 0) {
        // TODO: check if files empty
        track_changes(argc, argv);

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
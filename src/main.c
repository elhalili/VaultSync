#include "../include/vm/repository.h"
#include "../include/vm/file.h"
#include "../include/common.h"
#include "../include/vm/commit.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main() {
    struct repository *repo = create_repo();
    strcpy(repo->dir, "/home/elhalili/workspace/VaultSync/build");
    repo->dir[41] = 0;
    // if (get_status(repo) == FAIL) perror("? ");

    // struct repository *repo = create_repo();
    // if(init_repo(repo) == SUCCESS) {
    //     printf("The initialization done\n");
    // }
    
    struct file_node* files = NULL;

    files = addFile(files, "/home/elhalili/workspace/VaultSync/src/vm/file.c", "NULL");
    // files = addFile(files, "/home/elhalili/workspace/VaultSync/src/common.c", "NULL");


    if (add_files(repo, files) == SUCCESS)
    {
        printf("Add changes done\n");
    }
    
    return 0;
}




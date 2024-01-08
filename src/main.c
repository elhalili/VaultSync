#include "../include/vm/repository.h"
#include "../include/vm/file.h"
#include "../include/vm/common.h"
#include "../include/vm/commit.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main() {
    struct repository *repo = create_repo();
    strcpy(repo->dir, "/home/elhalili/workspace/VaultSync/build/mypr");
//    if (get_status(repo) == FAIL) perror("? ");
//
//    struct repository *repo = create_repo();
//    if(init_repo(repo) == SUCCESS) {
//        printf("The initialization done\n");
//    }
//    

    // this for making changes
//    struct file_node* files = NULL;
//    files = addFile(files, "/home/elhalili/workspace/VaultSync/build/mypr/se.txt", "NULL");
//    
//    if (add_files(repo, files) == SUCCESS)
//    {
//         printf("Add changes done\n");
//    }
//
    // Creating new commit
    struct commit *last_commit = (struct commit*) malloc(sizeof(struct commit));
    strcpy(last_commit->hash, "0");
    strcpy(last_commit->parent_hash, "-");
    
    repo->last_commit = last_commit;


    struct commit *new_commit = (struct commit*) malloc(sizeof(struct commit));

    if (make_commit(repo, new_commit) == SUCCESS)
    {
        printf("Commit done");
    }
    
//    return 0;
}




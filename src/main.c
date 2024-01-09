#include "../include/vm/repository.h"
#include "../include/vm/file.h"
#include "../include/vm/common.h"
#include "../include/vm/commit.h"
#include "../include/vm/author.h"
#include "../include/vm/file.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>


int main() {
    struct author* author = (struct author*) malloc(sizeof(struct author));
    strcpy(author->mail, "john@example.com");
    strcpy(author->username, "john");
    // Creating new commit
    struct repository* repo = NULL;
    
    // if (init_repo(repo, author) == SUCCESS) {
    //     printf("Init done\n");
    // }

    // adding changes
    struct file_node* file = NULL;
    file = addFile(file, "/home/elhalili/workspace/VaultSync/build/new.txt");

    repo = (struct repository*) malloc(sizeof(struct repository));
    repo->author = author;
    strcpy(repo->dir, "/home/elhalili/workspace/VaultSync/build");


    if (add_changes(repo, file) == SUCCESS) {
        printf("Add changes done\n");
    }
    

    return 0;
}




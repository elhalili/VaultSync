#include "../include/vm/repository.h"
#include "../include/common.h"
#include <stdio.h>
#include <stdlib.h>
int main() {
    
    struct repository *repo = create_repo();
    char* str = malloc(255);
    str = "/home/elhalili/workspace/VaultSync/build";
    repo->dir = str;
    if (get_status(repo) == FAIL) perror("? ");
    
    return 0;
}

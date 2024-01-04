#include "../include/common.h"
#include <stdio.h>


int create_file(const char* filename, const char* content) {
    FILE *file = fopen(filename, "w");
    if (file == NULL) return FAIL;
    if (fprintf(file, content) < 0) return FAIL;

    fclose(file);

    return SUCCESS;
}
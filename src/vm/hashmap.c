#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../../include/vm/hashmap.h"
#include "../../include/logging/logger.h"
#include "../../include/common.h"

unsigned int hash(const char* key) {
    unsigned int hash_value = 0;
    for (int i = 0; i < strlen(key); ++i) {
        hash_value = hash_value * 31 + key[i];
    }

    return hash_value % TABLE_SIZE;
}

void insert_map(struct hash_map* map, const char* key, const char* value) {
    unsigned int index = hash(key);

    struct key_value* newNode = (struct key_value*)malloc(sizeof(struct key_value));

    // Ensure that the size of newNode->key and newNode->value can accommodate the strings
    strncpy(newNode->key, key, MAX_PATH - 1);
    newNode->key[MAX_PATH - 1] = '\0';  // Set null terminator

    
    strncpy(newNode->value, value, HASH_LEN - 1);
    newNode->value[HASH_LEN - 1] = '\0';  // Set null terminator

    newNode->next = map->table[index];
    map->table[index] = newNode;
}


char* get_value_from_key(struct hash_map* map, const char* key) {
    unsigned int index = hash(key);
    
    struct key_value* current = map->table[index];
    while (current != NULL) {

        if (strcmp(current->key, key) == 0) {
            return current->value;
        }
        current = current->next;
    }

     
    return NULL;
}

void delete_from_map(struct hash_map* map, const char* key) {
    unsigned int index = hash(key);

    struct key_value* current = map->table[index];
    struct key_value* previous = NULL;

    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            if (previous == NULL) {
                map->table[index] = current->next;
            } else {
                previous->next = current->next;
            }

            free(current);
            return;
        }

        previous = current;
        current = current->next;
    }
}

void clear_hash_map(struct hash_map* map) {
    for (int i = 0; i < TABLE_SIZE; ++i) {
        struct key_value* current = map->table[i];
        while (current != NULL) {
            struct key_value* temp = current;
            current = current->next;
            free(temp);
        }
        map->table[i] = NULL;
    }
}


int populate_hashmap_from_file(struct hash_map* map, const char* filename) {
    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        logger(ERROR_TAG, "Can not open track file");
        return FAIL;
    }

    char line[HASH_LEN + 1 + MAX_PATH];
    while (fgets(line, sizeof(line), file) != NULL) {
        char* spacePos = strchr(line, ' ');
        if (spacePos != NULL) {
            *spacePos = '\0';  // Null-terminate the hash
            char* path = line;
            char* hash = spacePos + 1;
            hash[strlen(hash) - 1] = '\0';  // Remove the newline character

            insert_map(map, path, hash);
        }
    }

    fclose(file);
}
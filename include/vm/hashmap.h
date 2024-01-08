#ifndef HASHMAP_H
#define HASHMAP_H

#include <openssl/sha.h>

#define MAX_PATH 1024
#define HASH_LEN 2 * SHA256_DIGEST_LENGTH + 1


#define TABLE_SIZE 10

struct key_value {
    char key[MAX_PATH];
    char value[HASH_LEN];
    // for collisions
    struct key_value* next;
};

struct hash_map {
    struct key_value* table[TABLE_SIZE];
};

void init_hash_map(struct hash_map* map);

/**
 * calculate a simple hash value for the key
*/
unsigned int hash(const char* key);

/**
 * delete from hashmap
*/
void delete_from_map(struct hash_map* map, const char* key);
/**
 * insert a key-value pair into the hashmap
*/
void insert_map(struct hash_map* map, const char* key, const char* value);

/**
 * retrieve a value from the hashmap based on the key
*/
char* get_value_from_key(struct hash_map* map, const char* key);

/**
 * free the memory used by the hash_map
*/
void clear_hash_map(struct hash_map* map);

/**
 * creating a hashmap from a given file
*/
int populate_hashmap_from_file(struct hash_map* map, const char* filename);

#endif
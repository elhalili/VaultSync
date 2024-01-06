#include "../../include/crypto/hashing.h"
#include "../../include/vm/commit.h"
#include <stdio.h>
#include <openssl/sha.h>
#include <time.h>
#include <stdlib.h>

void generate_random_data(char *buffer, int length) {
    const char charset[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    size_t charset_size = sizeof(charset) - 1;

    for (size_t i = 0; i < length; i++) {
        buffer[i] = charset[rand() % charset_size];
    }

    buffer[length] = '\0';  // Null-terminate the string
}

void generate_hash(const char *data, int length, unsigned char *hash) {
    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, data, length);
    SHA256_Final(hash, &sha256);
}

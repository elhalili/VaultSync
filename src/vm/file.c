#include "../../include/vm/file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


struct file_node* addFile(struct file_node* head, const char* path, const char* hash) {
    struct file_node* newNode = (struct file_node*)malloc(sizeof(struct file_node));
    strcpy(newNode->path, path);
    strcpy(newNode->hash, hash);
    newNode->next = head;
    return newNode;
}

struct file_node* removeFileByHash(struct file_node* head, const char* hash) {
    struct file_node* current = head;
    struct file_node* previous = NULL;

    while (current != NULL) {
        if (strcmp(current->hash, hash) == 0) {
            if (previous == NULL) {
                head = current->next;
            } else {
                previous->next = current->next;
            }

            free(current);
            return head;
        }

        previous = current;
        current = current->next;
    }

    return head; 
}

struct file_node* removeFileByPath(struct file_node* head, const char* path) {
    struct file_node* current = head;
    struct file_node* previous = NULL;

    while (current != NULL) {
        if (strcmp(current->path, path) == 0) {
            if (previous == NULL) {
                head = current->next;
            } else {
                previous->next = current->next;
            }

            free(current);
            return head; // Return the updated head
        }

        previous = current;
        current = current->next;
    }

    return head;
}

struct file_node* findByHash(struct file_node* head, const char* hash) {
    struct file_node* current = head;

    while (current != NULL) {
        if (strcmp(current->hash, hash) == 0) {
            return current; 
        }

        current = current->next;
    }

    return NULL; 
}

struct file_node* findByPath(struct file_node* head, const char* path) {
    struct file_node* current = head;

    while (current != NULL) {
        if (strcmp(current->path, path) == 0) {
            return current; 
        }

        current = current->next;
    }

    return NULL;
}

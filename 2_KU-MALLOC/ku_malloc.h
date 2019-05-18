#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <stdbool.h>

typedef struct __hnode_t { //size 16
    size_t size;
    struct __hnode_t *next;
} hnode_t;

typedef struct __hheader_t { //size 16
    size_t size;
    unsigned long magic;
} hheader_t;

hnode_t *free_list = NULL;
hnode_t *alloc_last_node = NULL;

void remove_node(hnode_t** head, hnode_t* remove) {
    if (*head == remove) {
        *head = remove->next;
    } else {
        hnode_t* current = *head;
        while (current != NULL && current->next != remove) {
            current = current->next;
        }
        if (current != NULL)
            current->next = remove->next;
    }
}

/*
int get_node_count(hnode_t* head) {
    int count = 0;
    hnode_t* current = head;

    while (current != NULL) {
        current = current->next;
        count++;
    }
    return count;
}

hnode_t* get_node_at(hnode_t* head, int location) {
    hnode_t* current = head;
    while (current != NULL && (--location) >= 0) {
        current = current->next;
    }
    return current;
}
*/
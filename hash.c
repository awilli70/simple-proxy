/* Hash.h - Implementation of hash table with chaining  *
 * To be used with cache as fast access for cache queue *
 * Alexander Williams                                   */

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <time.h>
#include <string.h>
#include "hash.h"

#define H Hash_T
#define Q Queue_T
#define N Node_T

struct Q
{
    struct N *head;
    struct N *tail;
};

struct N
{
    char *key;
    char *data;
    u_int32_t ttl;
    time_t created;
    struct N *prev;
    struct N *next;
    struct N *list_ptr;
};

struct H
{
    u_int32_t size;
    u_int32_t curr_blocks;
    Queue_T *arr;
};

char *node_key(N n) {
    assert(n != NULL);
    return n->key;
}

void pretty_print_table(H h) {
    for (u_int32_t i = 0; i < h->size; i++) {
        pretty_print_queue(h->arr[i]);
    }
    printf("\n");
    return;
}

u_int32_t curr_blocks(H h) {
    assert(h != NULL);
    return h->curr_blocks;
}

u_int32_t hash_size(H h) {
    assert(h != NULL);
    return h->size;
}

struct N *list_ptr(N n) {
    assert(n != NULL);
    return n->list_ptr;
}

u_int32_t hash(char *key) {
    assert(key != NULL);
    u_int32_t hash = 5381;
    u_int32_t c;

    while ((c = *key++)) {
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
    }
    return hash;
}

struct H *initialize_table(u_int32_t size) {
    assert(size > 0);
    H h;
    h = malloc(sizeof(struct Hash_T));
    assert(h != NULL);
    h->arr = malloc(sizeof(struct Queue_T) * size);
    for (u_int32_t i = 0; i < size; i++) {
        h->arr[i] = initialize_queue();
    }
    h->size = size;
    h->curr_blocks = 0;
    assert(h->arr != NULL);
    return h;
}

void free_table(H h) {
    assert(h != NULL);
    for (u_int32_t i = 0; i < h->size; i++) {
        free_queue(h->arr[i]);
    }
    free(h->arr);
    free(h);
}

struct H *hash_insert(H h, char *key, N list_ptr) {
    assert((h != NULL && key != NULL) && list_ptr != NULL);
    Node_T check = hash_search(h, key);
    if (check != NULL) {
        assert(list_ptr == check->list_ptr);
        return h;
    } else {
        u_int32_t idx = hash(key) % h->size;
        Queue_T q = h->arr[idx];
        char *hash_key = malloc(sizeof(char) * 100);
        hash_key = strncpy(hash_key, key, sizeof(char) * 100);
        char *data = malloc(sizeof(char) * 2);
        data = strncpy(data, "d", (sizeof(char) * 2));
        q = push(q, hash_key, data, 0, list_ptr);
        h->curr_blocks += 1;
        return h;
    }
}

struct N *hash_search(H h, char *key) {
    u_int32_t idx = hash(key) % h->size;
    Queue_T q = h->arr[idx];
    return search_list(q, key);
}

struct H *hash_remove(H h, char *key) {
    assert(h != NULL && key != NULL);
    u_int32_t idx = hash(key) % h->size;
    Node_T n = search_list(h->arr[idx], key);
    if (n != NULL) {
        delete_node(h->arr[idx], n);
        h->curr_blocks = h->curr_blocks - 1;
    }
    return h;
}
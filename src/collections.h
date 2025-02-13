#ifndef COLLECTIONS_H
#define COLLECTIONS_H

#include<stdlib.h>
#include "utils.h"

/* DYNAMIC LIST */
#define LIST_DEFAULT_INITIAL_CAPACITY 10

typedef struct dynamic_list dynamic_list;
struct dynamic_list {
    size_t len, capacity;
    size_t type_size;
    void *list;
};

// This solely exists for readability. E.g. a struct can contain
//      list(char) symbols;
// Which makes it easier to determine the list ype
#define list(TYPE) struct dynamic_list*

#define new_list(TYPE)\
    _new_list(sizeof(TYPE), LIST_DEFAULT_INITIAL_CAPACITY)
#define list_new_with_capacity(TYPE, CAPACITY)\
    _new_list(sizeof(TYPE), CAPACITY)
void* _new_list(size_t type_size, size_t initial_capacity);

void delete_list(struct dynamic_list *list_ptr);

void _list_grow(struct dynamic_list *meta_data, size_t capacity);

#define list_push_rv(LIST, DATA)\
do{\
typeof(DATA) __r = DATA;\
list_push(LIST, &__r);\
}while(0)
void list_push(struct dynamic_list *list_ptr, void *data);


void list_pop(struct dynamic_list *list_ptr);

#define list_len(LIST) LIST->len
#define list_capacity(LIST) LIST->capacity

/* GENERIC COLLECTION QUEUE */
// Unique node sizes

typedef struct queue_node queue_node;
struct queue_node {
    void *data;
    struct queue_node *next;
};
#define queue(TYPE) queue*
typedef struct queue queue;
struct queue {
    struct queue_node *head, *tail;
    size_t len, type_size;
};

#define new_queue(TYPE) _new_queue(sizeof(TYPE))
queue* _new_queue(size_t type_size);
void delete_queue(queue *queue_ptr);

void* queue_poll(queue *queue_ptr);
void queue_pop(queue *queue_ptr);
void queue_add(queue *queue_ptr, void *data);

/* HASHMAP */
// TODO: Add support for string keys

#define HASHMAP_DEFAULT_INITIAL_CAPACITY 100
#define HASHMAP_PRIME_CONSTANT 37

struct hashmap_entry {
    byte *key, *value;
    struct hashmap_entry *next;
    bool empty;
};

typedef struct hashmap hashmap;
struct hashmap {
    size_t key_size, type_size, capacity, len;
    byte *entries;
};

#define new_hashmap(KEY, TYPE) _new_hashmap(sizeof(KEY), sizeof(TYPE))
struct hashmap* _new_hashmap(size_t key_size, size_t type_size);

void delete_hashmap(struct hashmap* hashmap_ptr);


#define hashmap_add(MAP, KEY, VALUE)\
    _hashmap_add(MAP, (byte*)KEY, (byte*)VALUE)

void _hashmap_add(struct hashmap *hashmap_ptr, byte *key, byte *value);

#define hashmap_remove(MAP, KEY) _hashmap_remove(MAP, (byte*)KEY)
void _hashmap_remove(struct hashmap *hashmap_ptr, byte *key);


#define hashmap_get(MAP, KEY) _hashmap_get(MAP, (byte*)KEY)
void* _hashmap_get(struct hashmap *hashmap_ptr, byte *key);


/* STRING VIEW */
// Technically not a collection, butttt
// a string is an array of chars.
// deal with it
typedef struct string_view string_view;
struct string_view {
    const char *str;
    size_t len;
};

bool string_view_cmp(string_view *sv1, string_view *sv2);


#endif /* COLLECTIONS_H */


#include "collections.h"

#include <stdio.h>
#include <string.h>

/* DYNAMIC LIST */
void* _new_list(size_t type_size, size_t initial_capacity) {
    if(type_size == 0 || initial_capacity == 0) {
        PANIC("Attempted to create list with type_size = 0 or initial_capacity = 0");
    }

    struct dynamic_list *list_ptr = (struct dynamic_list*)malloc(sizeof(struct dynamic_list));
    list_ptr->type_size = type_size;
    list_ptr->capacity = initial_capacity;
    list_ptr->list = malloc(type_size * initial_capacity);
    list_ptr->len = 0;
    memset(list_ptr->list, 0, type_size * initial_capacity);

    return list_ptr;
}

void delete_list(struct dynamic_list *list_ptr) {
    if(list_ptr == NULL) {
        PANIC("Attempted to free null pointer with delete_list()");
    }
    free(list_ptr->list);
    free(list_ptr);
}

void _list_grow(struct dynamic_list *list_ptr, size_t capacity) {
    if(capacity < list_ptr->capacity) {
        PANIC("Attempted to shrink list with _list_grow()");
    }

    list_ptr->list = realloc(list_ptr->list, capacity * list_ptr->type_size);

    memset(((char*)list_ptr->list) + (list_ptr->len * list_ptr->type_size),
           0, 
           (capacity - list_ptr->capacity) * list_ptr->type_size);

    list_ptr->capacity = capacity;
}

void list_push(struct dynamic_list *list_ptr, void *data) {
    if(data == NULL || list_ptr == NULL) {
        PANIC("NULL pointer as argument for list_push()");
    }

    if(list_ptr->len >= list_ptr->capacity) {
        _list_grow(list_ptr, list_ptr->capacity * 2);
    }

    memcpy(list_ptr->list + (list_ptr->len * list_ptr->type_size), data, list_ptr->type_size);
    list_ptr->len += 1;
}

void list_pop(struct dynamic_list *list_ptr) {
    list_ptr->len -= 1;
}

/* QUEUE */

queue* _new_queue(size_t type_size) {
    queue *q = (queue*)calloc(1, sizeof(struct queue));
    q->type_size = type_size;

    return q;
}
void delete_queue_node(struct queue_node *node) {
    if(node->next != NULL) delete_queue_node(node->next);
    free(node);
}
void delete_queue(queue *queue_ptr) {
    if(queue_ptr->head != NULL) delete_queue_node(queue_ptr->head);
    free(queue_ptr);
}
void* queue_poll(queue *queue_ptr) {
    if(queue_ptr->head == NULL) return NULL;
    return queue_ptr->head->data;
}
void queue_pop(queue *queue_ptr) {
    if(queue_ptr == NULL) {
        PANIC("Called queue_pop() with NULL pointer");
    }else if(queue_ptr->len == 0) {
        PANIC("Attempted to queue_pop() with empty queue");
    }

    void *ptr = queue_ptr->head;

    if(queue_ptr->len == 1) {
        queue_ptr->head = NULL;
        queue_ptr->tail = NULL;
    }else {
        queue_ptr->head = queue_ptr->head->next;
    }
    queue_ptr->len--;

    free(ptr);
}
void queue_add(queue *queue_ptr, void *data) {
    if(queue_ptr == NULL || data == NULL) {
        PANIC("Called queue_add() with NULL pointer");
    }
    queue_ptr->len++;

    struct queue_node *ptr = malloc(sizeof(struct queue_node) + queue_ptr->type_size);
    ptr->data = ((char*)ptr) + sizeof(struct queue_node);
    ptr->next = NULL;
    memcpy(ptr->data, data, queue_ptr->type_size);

    if(queue_ptr->head == NULL) {
        queue_ptr->head = ptr;
        queue_ptr->tail = ptr;
    }else {
        queue_ptr->tail->next = ptr;
        queue_ptr->tail = ptr;
    }
}

/* HASHMAP */

static inline
struct hashmap_entry* _hashmap_get_entry(struct hashmap *hashmap_ptr, size_t index) {
    return (struct hashmap_entry*)(hashmap_ptr->entries 
    + (( sizeof(struct hashmap_entry) + hashmap_ptr->key_size + hashmap_ptr->type_size) * index  ));
}

struct hashmap* _new_hashmap(size_t key_size, size_t type_size) {
    struct hashmap *map = calloc(1, sizeof(struct hashmap));

    map->key_size = key_size;
    map->type_size = type_size;
    map->capacity = HASHMAP_DEFAULT_INITIAL_CAPACITY; /* 100 */
    map->entries = calloc(map->capacity, sizeof(struct hashmap_entry) + key_size + type_size);
    
    for(size_t i = 0; i < map->capacity; i++) {
        struct hashmap_entry *e = _hashmap_get_entry(map, i);
        e->key = ((byte*)e) + sizeof(struct hashmap_entry);
        e->value = ((byte*)e->key) + key_size;
        e->empty = true;
    }

    return map;
}

void _delete_hashmap_entries(struct hashmap_entry *entry) {
    if(entry == NULL) return;
    _delete_hashmap_entries(entry->next);
    free(entry);
}

void delete_hashmap(struct hashmap *hashmap_ptr) {
    
    for(size_t i = 0; i < hashmap_ptr->capacity; i++) {
        _delete_hashmap_entries(_hashmap_get_entry(hashmap_ptr, i)->next);
    }
    free(hashmap_ptr->entries);
    free(hashmap_ptr);
}

// NOTE: This is intentionally returns a when n is zero
size_t _hash_stpow(size_t a, size_t n) {
    for(size_t i = 0; i < n; i++) a *= a;
    return a;
}

size_t _hash(byte *data, size_t size, size_t capacity) {
    size_t index = 0;

    for(size_t i = 0; i < size; i++)
        index += data[i] * _hash_stpow(HASHMAP_PRIME_CONSTANT, i);

    return index % capacity;
}

void _hashmap_resize(struct hashmap *hashmap_ptr, size_t capacity) {
    (void)hashmap_ptr; (void)capacity;
    PANIC("_hashmap_resize() is not implemented yet");
}

struct hashmap_entry* _hashmap_entry_alloc(struct hashmap *hashmap_ptr) {
    struct hashmap_entry* new_entry = calloc(1, sizeof(struct hashmap_entry) 
                                             + hashmap_ptr->key_size 
                                             + hashmap_ptr->type_size);
    new_entry->key = ((byte*)new_entry) + sizeof(struct hashmap_entry);
    new_entry->value = ((byte*)new_entry->key) + hashmap_ptr->key_size;
    new_entry->next = NULL;
    new_entry->empty = true;

    return new_entry;
}

void _hashmap_add(struct hashmap *hashmap_ptr, byte *key, byte *value) {
    if(key == NULL || value == NULL || hashmap_ptr == NULL) {
        PANIC("NULL pointer as argument for hashmap_add()");
    }

    if(hashmap_ptr->len >= hashmap_ptr->capacity * 3/4) {
        _hashmap_resize(hashmap_ptr, hashmap_ptr->capacity * 2);
    }

    size_t index = _hash(key, hashmap_ptr->key_size, hashmap_ptr->capacity);
    struct hashmap_entry *entry_ptr = _hashmap_get_entry(hashmap_ptr, index);

    //LOG("INDEX: %zu, Key arg: %d. Entry key: %d, Entry value: %f",index, *(int*)key, *(int*)entry_ptr->key, *(float*)entry_ptr->value);
    if(!entry_ptr->empty && memcmp(entry_ptr->key, key, hashmap_ptr->key_size) != 0) {

        LOG("Already in use");
        while(entry_ptr->next != NULL) {
            entry_ptr = entry_ptr->next;
        }
        entry_ptr->next = _hashmap_entry_alloc(hashmap_ptr);
        entry_ptr = entry_ptr->next;
    }

    memcpy(entry_ptr->key, key, hashmap_ptr->key_size);
    memcpy(entry_ptr->value, value, hashmap_ptr->type_size);
    entry_ptr->empty = false;
}

void _hashmap_remove(struct hashmap *hashmap_ptr, byte *key) {
    if(key == NULL || hashmap_ptr == NULL) {
        PANIC("NULL pointer as argument for hashmap_remove()");
    }

    size_t index = _hash(key, hashmap_ptr->key_size, hashmap_ptr->capacity);

    struct hashmap_entry *prev = NULL;
    struct hashmap_entry *entry = _hashmap_get_entry(hashmap_ptr, index);

    while(entry != NULL && memcmp(entry->key, key, hashmap_ptr->key_size) != 0) {
        prev = entry;
        entry = entry->next;
    }

    if(entry == NULL) {
        PANIC("Attempted to remove key that does not exist with hashmap_remove()");
    }

    if(prev != NULL) {
        struct hashmap_entry *tmp = entry->next;
        free(entry);
        prev->next = tmp;
    }else {
        //memset(((byte*)entry) + sizeof(struct hashmap_entry), 0, hashmap_ptr->key_size + hashmap_ptr->type_size);
        entry->empty = true;
    }
}

void* _hashmap_get(struct hashmap *hashmap_ptr, byte *key) {
    if(key == NULL || hashmap_ptr == NULL) {
        PANIC("NULL pointer as argument for hashmap_get()");
    }

    size_t index = _hash(key, hashmap_ptr->key_size, hashmap_ptr->capacity);
    struct hashmap_entry *entry = _hashmap_get_entry(hashmap_ptr, index);

    while(entry != NULL && memcmp(entry->key, key, hashmap_ptr->key_size) != 0) {
        entry = entry->next;
    }
    if(entry == NULL || entry->empty) {
        return NULL;
    }

    return entry->value;
}

// STRING_VIEW

bool string_view_cmp(string_view *sv1, string_view *sv2) {
    return sv1->len == sv2->len && strncmp(sv1->str, sv2->str, sv1->len) == 0;
}

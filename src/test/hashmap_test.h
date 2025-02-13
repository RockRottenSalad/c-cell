#ifndef HASHMAP_TEST_H
#define HASHMAP_TEST_H

#include "../collections.h"
#include "../utils.h"
#include "test.h"
#include <stdio.h>

bool insertion_test() {
    hashmap *map = new_hashmap(int, float);
    
    int key = 52;
    float value = 3.14;

    hashmap_add(map, &key, &value);
    float result = *(float*)hashmap_get(map, &key);

    ASSERT(result == value, "Insertion test failed, expected value: %f, got %f", value, result);

    value = 400.0;
    hashmap_add(map, &key, &value);
    result = *(float*)hashmap_get(map, &key);

    ASSERT(result == value, "Insertion test failed, expected value: %f, got %f", value, result);

    delete_hashmap(map);

    return true;
}

bool removal_test() {
    hashmap *map = new_hashmap(int, float);
    
    int key = 52;
    float value = 3.14;

    hashmap_add(map, &key, &value);
    float result = *(float*)hashmap_get(map, &key);

    ASSERT(result == value, "Removal test failed, expected value: %f, got %f", value, result);

    hashmap_remove(map, &key);
    ASSERT(NULL == hashmap_get(map, &key), "Removal test failed, expected value: NULL");

    delete_hashmap(map);

    return true;
}

void hashmap_test(void) {

    LOG("Starting hashmap test");

    RUN_TEST(insertion_test(), "Hashmap insertion test" );
    RUN_TEST(removal_test(), "Hashmap removal test" );
}


#endif /* HASHMAP_TEST_H */

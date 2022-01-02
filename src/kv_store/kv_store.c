//
// Created by Joey Kraut on 12/30/21.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "kv_store.h"

/**
 * HELPERS
 */

int compare_keys(const void *a, const void *b, void *data)
{
    const kv_pair_t *pair1 = a;
    const kv_pair_t *pair2 = b;

    return strcmp(pair1->key, pair2->key);
}

uint64_t hash_key(const void *item, uint64_t seed0, uint64_t seed1)
{
    const kv_pair_t *pair = item;
    return hashmap_sip(pair->key, strlen(pair->key), seed0, seed1);
}


/**
 * KV STORE INTERFACE
 */

kv_store_t *create_kv_store()
{
    kv_store_t *kv_store = malloc(sizeof(kv_store_t));
    if (kv_store == NULL) {
        goto error1;
    }

    kv_store->map = hashmap_new(sizeof(kv_pair_t), 0, 0, 0,
            hash_key, compare_keys, NULL, NULL);
    if (kv_store->map == NULL) {
        goto error2;
    }

    kv_store->lock_manager = create_lock_manager();
    if (kv_store->lock_manager == NULL) {
        goto error3;
    }

    return kv_store;

error3:
    hashmap_free(kv_store->map);
error2:
    free(kv_store);
error1:
    return NULL;
}

void free_kv_store(kv_store_t *store)
{
    // Free the hashmap backing the data structure
    hashmap_free(store->map);
    free_lock_manager(store->lock_manager);
    free(store);
}

// Fetch the value for the given key
const char *kv_get(kv_store_t *store, const char *key)
{
    kv_pair_t pair = { .key = key };

    lock_manager_read_lock(store->lock_manager, key);
    void *result = hashmap_get(store->map, &pair);
    lock_manager_unlock(store->lock_manager, key);

    if (result == NULL) {
        return NULL;
    }

    return ((kv_pair_t *) result)->value;
}

// Set the value of the given key to the given value
const char *kv_set(kv_store_t *store, const char *key, const char *value)
{
    // Construct a kv pair
    kv_pair_t pair = {
            .key = key,
            .value = value,
    };

    lock_manager_write_lock(store->lock_manager, key);
    void *result = hashmap_set(store->map, &pair);
    lock_manager_unlock(store->lock_manager, key);

    if (result == NULL) {
        return NULL;
    }

    return ((kv_pair_t *) result)->value;
}
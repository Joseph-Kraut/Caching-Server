//
// Created by Joey Kraut on 12/30/21.
// Wraps a hashmap with thread safe protection
//

#ifndef CACHING_SERVER_KV_STORE_H
#define CACHING_SERVER_KV_STORE_H

#include "hashmap.h"

typedef struct kv_store {
    struct hashmap *map;
} kv_store_t;

typedef struct kv_pair {
    const char *key;
    const char *value;
} kv_pair_t;

// Interface
kv_store_t *create_kv_store();
void free_kv_store(kv_store_t *store);
const char *kv_get(kv_store_t *store, const char *key);
const char *kv_set(kv_store_t *store, const char *key, const char *value);

#endif //CACHING_SERVER_KV_STORE_H

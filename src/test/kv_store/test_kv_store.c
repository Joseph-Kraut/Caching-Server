//
// Created by Joey Kraut on 1/2/22.
//
#include <stdio.h>
#include <string.h>

#include "kv_store.h"
#include "helpers.h"
#include "test_kv_store.h"

void test_create_and_free()
{
    kv_store_t *store = create_kv_store();
    if (store == NULL) {
        goto fail;
    }
    free_kv_store(store);

    print_success("test_create_and_free");
    return;

fail:
    print_failure("test_create_and_free");
}

void test_set_and_get()
{
    kv_store_t *store = create_kv_store();
    if (store == NULL) {
        goto fail;
    }

    // Assert unset key returns null
    const char *value = kv_get(store, "key");
    if (value != NULL) {
        goto fail;
    }

    // Set key and verify its value
    const char *old_value = kv_set(store, "key", "value");
    if (old_value != NULL) {
        goto fail;
    }

    const char *new_value = kv_get(store, "key");
    if (strcmp("value", new_value)) {
        goto fail;
    }

    free_kv_store(store);

    print_success("test_set_and_get");
    return;

fail:
    print_failure("test_set_and_get");
}
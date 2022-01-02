//
// Created by Joey Kraut on 1/2/22.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "lock_manager.h"

/**
 * HELPERS
 */
uint64_t hash(const char *key)
{
    return hashmap_sip(key, strlen(key), 0, 0);
}

/**
 * INTERFACE
 */
lock_manager_t *create_lock_manager()
{
    lock_manager_t *manager = malloc(sizeof(lock_manager_t));
    if (manager == NULL) {
        goto error1;
    }
    manager->num_locks = LOCK_ARRAY_SIZE;

    // Create readers writer locks
    manager->rw_locks = malloc(manager->num_locks * sizeof(pthread_rwlock_t *));
    if (manager->rw_locks == NULL) {
        goto error2;
    }

    int i = 0;
    for (; i < manager->num_locks; i++) {
        manager->rw_locks[i] = malloc(sizeof(pthread_rwlock_t));
        if (manager->rw_locks[i] == NULL) {
            goto free_locks;
        }

        if (pthread_rwlock_init(manager->rw_locks[i], NULL)) {
            free(manager->rw_locks[i]);
            goto free_locks;
        }
    }

    return manager;

free_locks:
    for (int j = 0; j < i; j++) {
        free(manager->rw_locks[j]);
    }
error2:
    free(manager);
error1:
    return NULL;
}

void free_lock_manager(lock_manager_t *manager)
{
    // Free the readers writer locks
    for (int i = 0; i < manager->num_locks; i++) {
        pthread_rwlock_destroy(manager->rw_locks[i]);
        free(manager->rw_locks[i]);
    }

    // Free the lock manager
    free(manager);
}

int lock_manager_read_lock(lock_manager_t *manager, const char *key)
{
    // Hash the key to get the appropriate lock
    int lock_index = hash(key) % manager->num_locks;
    return pthread_rwlock_rdlock(manager->rw_locks[lock_index]);
}

int lock_manager_write_lock(lock_manager_t *manager, const char *key)
{
    // Hash the key to get the appropriate lock
    int lock_index = hash(key) % manager->num_locks;
    return pthread_rwlock_wrlock(manager->rw_locks[lock_index]);
}

int lock_manager_unlock(lock_manager_t *manager, const char *key)
{
    // Hash the key to get the appropriate lock
    int lock_index = hash(key) % manager->num_locks;
    return pthread_rwlock_unlock(manager->rw_locks[lock_index]);
}
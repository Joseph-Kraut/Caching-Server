//
// Created by Joey Kraut on 1/2/22.
//

#ifndef CACHING_SERVER_LOCK_MANAGER_H
#define CACHING_SERVER_LOCK_MANAGER_H

#include <pthread.h>

#include "hashmap.h"

#ifndef LOCK_ARRAY_SIZE
#define LOCK_ARRAY_SIZE 100
#endif //LOCK_ARRAY_SIZE

// Type
typedef struct lock_manager {
    int num_locks;
    pthread_rwlock_t **rw_locks;
} lock_manager_t;

// Interface
lock_manager_t *create_lock_manager();
void free_lock_manager(lock_manager_t *manager);

int lock_manager_read_lock(lock_manager_t *manager, const char *key);
int lock_manager_write_lock(lock_manager_t *manager, const char *key);
int lock_manager_unlock(lock_manager_t *manager, const char *key);

#endif //CACHING_SERVER_LOCK_MANAGER_H

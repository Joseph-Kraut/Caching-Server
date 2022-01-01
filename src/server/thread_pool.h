//
// Created by Joey Kraut on 12/31/21.
//

#ifndef CACHING_SERVER_THREAD_POOL_H
#define CACHING_SERVER_THREAD_POOL_H

#include <pthread.h>

typedef struct task {
   void (*fun)(void*);
   void *args;
} task_t;

typedef struct work_queue {
    // Work queue
    int max_queue_size;
    int queue_length;
    int queue_head;
    int queue_tail;
    pthread_mutex_t *mu;

    // Signalling
    pthread_cond_t *work_ready;

    task_t **tasks;
} work_queue_t;

typedef struct thread_pool {
    // Work queue
    work_queue_t *work_queue;

    // Threads
    int num_threads;
    pthread_t **threads;
} thread_pool_t;

// Thread Pool Interface
thread_pool_t *create_thread_pool(int num_threads, int max_queue_length);
void start_thread_pool(thread_pool_t *thread_pool);

void free_thread_pool(thread_pool_t *thread_pool);

int enqueue_task(thread_pool_t *pool, task_t *task);

#endif //CACHING_SERVER_THREAD_POOL_H

//
// Created by Joey Kraut on 12/31/21.
// Maintains a thread pool for serving requests
//
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "thread_pool.h"

/**
 * HELPERS
 */

void free_task(task_t *task)
{
    free(task->args);
    free(task);
}


/**
 * THREAD POOL INTERFACE
 */

thread_pool_t *create_thread_pool(int num_threads, int max_queue_length)
{
    thread_pool_t *pool = malloc(sizeof(thread_pool_t));
    if (pool == NULL) {
        goto err;
    }

    // Work queue allocation
    work_queue_t *queue = malloc(sizeof(work_queue_t));
    if (queue == NULL) {
        goto free1;
    }

    queue->max_queue_size = max_queue_length;
    queue->queue_length = 0;
    queue->queue_head = 0;
    queue->queue_tail = 0;

    queue->mu = malloc(sizeof(pthread_mutex_t));
    if (queue->mu == NULL) {
        goto free2;
    }
    if (pthread_mutex_init(queue->mu, NULL)) {
        goto free3;
    }

    queue->work_ready = malloc(sizeof(pthread_cond_t));
    if (queue->work_ready == NULL) {
        goto free3;
    }
    if (pthread_cond_init(queue->work_ready, NULL)) {
        goto free4;
    }

    queue->tasks = malloc(max_queue_length * sizeof(task_t *));
    if (queue->tasks == NULL) {
        goto free4;
    }
    pool->work_queue = queue;

    // Allocate threads for thread pool
    pool->num_threads = num_threads;
    pool->threads = malloc(num_threads * sizeof(pthread_t *));
    if (pool->threads == NULL) {
        goto free5;
    }

    int i = 0;
    for (; i < num_threads; i++) {
        pool->threads[i] = malloc(sizeof(pthread_t));
        if (pool->threads[i] == NULL) {
            goto free_threads;
        }
    }

    return pool;

free_threads:
    for (int j = 0; j < i; j++) {
        free(pool->threads[j]);
    }
free5:
    free(queue->tasks);
free4:
    free(queue->work_ready);
free3:
    free(queue->mu);
free2:
    free(queue);
free1:
    free(pool);
err:
    return NULL;
}

void free_thread_pool(thread_pool_t *thread_pool)
{
    // Free threads in the pool
    for (int i = 0; i < thread_pool->num_threads; i++) {
        free(thread_pool->threads[i]);
    }
    free(thread_pool->threads);

    // Free all remaining tasks
    for (
            int curr = thread_pool->work_queue->queue_head;
            curr < thread_pool->work_queue->queue_tail;
            curr = (curr + 1) % thread_pool->work_queue->max_queue_size
    ) {
        free_task(thread_pool->work_queue->tasks[curr]);
    }

    // Free work queue
    free(thread_pool->work_queue->tasks);
    free(thread_pool->work_queue->work_ready);
    free(thread_pool->work_queue->mu);
    free(thread_pool->work_queue);

    // Finally, free the thread pool
    free(thread_pool);
}

void *worker_routine(void *args)
{
    int err_val;
    if (args == NULL) {
        exit(EXIT_FAILURE);
    }

    work_queue_t *work_queue = args;
    while(1) {
        // Wait for new work to be added to queue
        pthread_mutex_lock(work_queue->mu);
        while (work_queue->queue_length == 0) {
            if ((err_val = pthread_cond_wait(work_queue->work_ready, work_queue->mu))) {
                goto exit;
            }
        }

        // Move the head
        work_queue->queue_length -= 1;

        task_t *curr_task = work_queue->tasks[work_queue->queue_head];
        work_queue->queue_head = (work_queue->queue_head + 1) % work_queue->max_queue_size;

        pthread_mutex_unlock(work_queue->mu);

        // Process task
        (*curr_task->fun)(curr_task->args);

        // Dealloc task
        free_task(curr_task);
    }

exit:
    printf("Thread exited with error %d...\n", err_val);
    return NULL;
}

void start_thread_pool(thread_pool_t *thread_pool)
{
    for (int i = 0; i < thread_pool->num_threads; i++) {
        pthread_create(thread_pool->threads[i], NULL, worker_routine, thread_pool->work_queue);
    }
}

int enqueue_task(thread_pool_t *pool, task_t *task)
{
    // Add a task to the tail if possible
    if (pool->work_queue->queue_length == pool->work_queue->max_queue_size) {
        errno = EAGAIN;
        return -1;
    }

    // Lock and add to queue
    pthread_mutex_lock(pool->work_queue->mu);

    int tail = pool->work_queue->queue_tail;
    pool->work_queue->tasks[tail] = task;
    pool->work_queue->queue_tail = (tail + 1) % pool->work_queue->max_queue_size;
    pool->work_queue->queue_length += 1;

    pthread_mutex_unlock(pool->work_queue->mu);
    pthread_cond_signal(pool->work_queue->work_ready);

    return 0;
}
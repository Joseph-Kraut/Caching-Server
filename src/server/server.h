//
// Created by Joey Kraut on 12/29/21.
//

#ifndef CACHING_SERVER_SERVER_H
#define CACHING_SERVER_SERVER_H

#include "kv_store.h"
#include "thread_pool.h"

#ifndef PORT
#define PORT 8000
#endif //PORT

#ifndef MAX_TCP_QUEUE
#define MAX_TCP_QUEUE 100
#endif //MAX_TCP_QUEUE

#ifndef MAX_REQUEST_LENGTH
#define MAX_REQUEST_LENGTH 4096
#endif //MAX_REQUEST_LENGTH

#ifndef THREAD_POOL_SIZE
#define THREAD_POOL_SIZE 2
#endif //THEAD_POOL_SIZE

#ifndef WORK_QUEUE_LENGTH
#define WORK_QUEUE_LENGTH 100 // Defaults to same size as TCP queue
#endif //WORK_QUEUE_LENGTH

// Server
typedef struct kv_server {
    int socket_fd;
    kv_store_t *kv_store;
    thread_pool_t *thread_pool;
} kv_server_t;

// Threading task args
typedef struct request_task_args {
     int connection_fd;
     kv_store_t *store;
} request_task_args_t;

// Interface
kv_server_t *create_server();
void free_server(kv_server_t *);
int listen_and_serve(kv_server_t *);
void handler(void *);

#endif //CACHING_SERVER_SERVER_H



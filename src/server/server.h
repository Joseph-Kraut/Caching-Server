//
// Created by Joey Kraut on 12/29/21.
//

#ifndef CACHING_SERVER_SERVER_H
#define CACHING_SERVER_SERVER_H

#include "kv_store.h"

#ifndef PORT
#define PORT 8000
#endif //PORT

#ifndef MAX_TCP_QUEUE
#define MAX_TCP_QUEUE 100
#endif //MAX_TCP_QUEUE

#ifndef MAX_REQUEST_LENGTH
#define MAX_REQUEST_LENGTH 4096
#endif //MAX_REQUEST_LENGTH

typedef struct kv_server {
    int socket_fd;
    kv_store_t *kv_store;
} kv_server_t;

// Interface
kv_server_t *create_server();
void free_server(kv_server_t *);
int listen_and_serve(kv_server_t *);
int handler(kv_server_t *, int);

#endif //CACHING_SERVER_SERVER_H



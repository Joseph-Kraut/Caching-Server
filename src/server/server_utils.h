//
// Created by Joey Kraut on 12/29/21.
//
#include "server.h"
#include "types.h"

#ifndef CACHING_SERVER_SERVER_UTILS_H
#define CACHING_SERVER_SERVER_UTILS_H

// Utils interface
int parse_request(char *, request_t *);
int create_socket();

#endif //CACHING_SERVER_SERVER_UTILS_H

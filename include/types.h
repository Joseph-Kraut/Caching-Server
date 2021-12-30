//
// Created by Joey Kraut on 12/29/21.
//

#ifndef CACHING_SERVER_TYPES_H
#define CACHING_SERVER_TYPES_H

typedef enum operation {
    GET,
    SET
} operation_t;

typedef struct request {
    uint8_t op_type;
    char *key;
    char *value;
} request_t;

#endif //CACHING_SERVER_TYPES_H

//
// Created by Joey Kraut on 12/29/21.
//

#ifndef CACHING_SERVER_TYPES_H
#define CACHING_SERVER_TYPES_H

#ifndef MAX_RESPONSE_SIZE
#define MAX_RESPONSE_SIZE 4608
#endif //MAX_RESPONSE_SIZE

typedef enum operation {
    GET,
    SET
} operation_t;

typedef enum response_status {
    OK,
    NOT_FOUND,
    INTERNAL_ERROR,
} response_status_t;

typedef struct request {
    uint8_t op_type;
    char *key;
    char *value;
} request_t;

typedef struct response {
    uint8_t response_status;
    const char *key;
    const char *value;
    const char *error_string;
} response_t;

#endif //CACHING_SERVER_TYPES_H

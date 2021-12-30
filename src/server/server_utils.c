//
// Created by Joey Kraut on 12/29/21.
//

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "server_utils.h"


int parse_get_request(char *request_buffer, request_t *result)
{
    result->op_type = GET;
    // First byte -> opcode
    // Rest of the bytes in GET are a string
    char *cursor = request_buffer + 1;
    result->key = cursor;

    return 0;
}

int parse_set_request(char *request_buffer, request_t *result)
{
    result->op_type = SET;
    // First byte -> opcode
    char *cursor = request_buffer + 1;
    result->key = cursor;

    cursor += strlen(result->key) + 1;
    result->value = cursor;

    return 0;
}

int parse_request(char *request_buffer, request_t *result)
{
    // Parse the operation type
    uint8_t op_code = *((uint8_t *) request_buffer);

    switch (op_code) {
        case GET:
            return parse_get_request(request_buffer, result);
        case SET:
            return parse_set_request(request_buffer, result);
        default:
            perror("Illegal operation");
            return -1;
    }
}


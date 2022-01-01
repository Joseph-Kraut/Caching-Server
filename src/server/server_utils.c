//
// Created by Joey Kraut on 12/29/21.
//

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include "server_utils.h"

/**
 * SERVER CREATION HELPERS
 */

int create_socket()
{
    int socket_fd;
    struct sockaddr_in socket_address;
    socklen_t socket_length;

    // Allocate a socket
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("error creating socket");
        return -1;
    }

    // Bind to an address
    socket_address.sin_family = AF_INET;
    socket_address.sin_port = htons(PORT);
    socket_address.sin_addr.s_addr = INADDR_ANY;

    socket_length = sizeof(socket_address);

    if (bind(socket_fd, (struct sockaddr *)&socket_address, socket_length)) {
        perror("error binding socket to address");
        goto error;
    }

    // Listen for connections
    if (listen(socket_fd, MAX_TCP_QUEUE)) {
        perror("error marking socket as passive");
        goto error;
    }

    return socket_fd;
error:
    close(socket_fd);
    return -1;
}

/**
 * REQUEST PARSING
 */
int parse_get_request(char *request_buffer, request_t *result)
{
    result->op_type = GET;
    // First few bytes are the opcode
    // Rest of the bytes in GET are a string
    char *cursor = request_buffer + sizeof(result->op_type);
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


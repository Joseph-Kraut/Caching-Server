//
// Created by Joey Kraut on 12/29/21.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "client.h"
#include "types.h"

int send_request(int socket_fd, request_t *request) {
    // Allocate a buffer for the request
    size_t key_size = strlen(request->key) + 1, val_size = strlen(request->value) + 1;
    size_t message_size = sizeof(request->op_type) + key_size + val_size;
    char *message_buffer = malloc(message_size);

    // Marshall request into buffer
    memcpy(message_buffer, &request->op_type, sizeof(request->op_type));
    memcpy(message_buffer + sizeof(request->op_type), request->key, key_size);
    memcpy(message_buffer + sizeof(request->op_type) + key_size, request->value, val_size);

    // Send the request
    if (write(socket_fd, message_buffer, message_size) != message_size) {
        perror("error sending message");
        return -1;
    }

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        perror("Usage: ./client <server_ip> <key> <value>");
        exit(EXIT_FAILURE);
    }

    int socket_fd;
    struct sockaddr_in server_addr;
    socklen_t server_addr_length = sizeof(server_addr);

    char *server_ip = argv[1], *key = argv[2], *value = argv[3];

    // Create a socket for messaging
    if ((socket_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("error creating socket");
        exit(EXIT_FAILURE);
    }

    // Connect to the address
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, server_ip, &server_addr.sin_addr.s_addr);

    if (connect(socket_fd, (struct sockaddr *)&server_addr, server_addr_length)) {
        perror("error connecting to server");
        exit(EXIT_FAILURE);
    }

    // Write to address
    request_t req = {
            .op_type = GET,
            .key = key,
            .value = value,
    };

    if (send_request(socket_fd, &req)) {
        perror("error sending request");
        exit(EXIT_FAILURE);
    }

    return 0;
}
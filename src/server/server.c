#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include "server.h"
#include "server_utils.h"

int main(int argc, char *argv[])
{
    // TODO create a thread pool for serving

    // Start a TCP server
    return start_tcp_server();
}

// Server connection handler
int start_tcp_server()
{
    int socket_fd, conn_fd;
    struct sockaddr_in socket_address, client_addr;
    socklen_t socket_length, client_addr_length = sizeof(client_addr);

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
        return -1;
    }

    // Listen for connections
    if (listen(socket_fd, MAX_TCP_QUEUE)) {
        perror("error marking socket as passive");
        return -1;
    }

    // Accept connections in loop
    while (1) {
        if ((conn_fd = accept(socket_fd, (struct sockaddr *)&client_addr, &client_addr_length)) == -1) {
            perror("error awaiting socket connections");
            return -1;
        }

        // Forward the request to the handler
        handler(conn_fd);
    }
}


/**
 * REQUEST HANDLERS
 */

int handler(int connection_fd)
{
    char *request_buffer = malloc(MAX_REQUEST_LENGTH);
    request_t *request = malloc(sizeof(request_t));

    // Read the request and parse it into struct
    if (read(connection_fd, request_buffer, MAX_REQUEST_LENGTH) == 0) {
        perror("error reading from connection");
        goto error;
    }

    // Parse the request
    if (parse_request(request_buffer, request)) {
        perror("error parsing request...");
        goto error;
    }

    printf("Got request with \n\tkey: %s\n\tvalue: %s\n", request->key, request->value);

error:
    free(request);
    free(request_buffer);
    return -1;
}
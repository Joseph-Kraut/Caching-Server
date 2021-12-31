#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>

#include "kv_store.h"
#include "server.h"
#include "server_utils.h"

/**
 * SERVER INTERFACE
 */
// Allocates the server and its data structures
kv_server_t *create_server()
{
    kv_server_t *server = malloc(sizeof(kv_server_t));

    // Open a socket and start kv store
    if ((server->socket_fd = create_socket()) == -1) {
        return 0x0;
    }
    server->kv_store = create_kv_store();

    return server;
}

// Frees the memory backing the server and its data structures
void free_server(kv_server_t *server)
{
    close(server->socket_fd);
    free_kv_store(server->kv_store);
}

// Server event listener
int listen_and_serve(kv_server_t *server)
{
    int conn_fd;
    struct sockaddr_in client_addr;
    socklen_t client_addr_length = sizeof(client_addr);

    // Accept connections in loop
    while (1) {
        if ((conn_fd = accept(server->socket_fd, (struct sockaddr *)&client_addr, &client_addr_length)) == -1) {
            perror("error awaiting socket connections");
            return -1;
        }

        // Forward the request to the handler
        handler(server, conn_fd);
    }
}


/**
 * REQUEST HANDLERS
 */
int send_response(int connection_fd, response_t *response) {
    // Marshall response into buffer
    size_t stat_length = sizeof(response->response_status);
    size_t key_length = (response->key == NULL) ? 1 : strlen(response->key) + 1;
    size_t val_length = (response->value == NULL) ? 1 : strlen(response->value) + 1;
    size_t err_length = strlen(response->error_string) + 1;
    size_t message_size = stat_length + key_length + val_length + err_length;

    char *response_buffer = malloc(message_size);
    memcpy(response_buffer, response, stat_length);

    if (response->key == NULL) {
        memcpy(response_buffer + stat_length, "\0", key_length);
    } else {
        memcpy(response_buffer + stat_length, response->key, key_length);
    }

    if (response->value == NULL) {
        memcpy(response_buffer + stat_length + key_length, "\0", val_length);
    } else {
        memcpy(response_buffer + stat_length + key_length, response->value, val_length);
    }

    memcpy(response_buffer + stat_length + key_length + val_length, response->error_string, err_length);

    // Send the response on the socket
    if (write(connection_fd, response_buffer, message_size) != message_size) {
        perror("error sending response");
        free(response_buffer);
        return -1;
    }

    free(response_buffer);
    return 0;
}

int handle_get_request(kv_server_t *server, int connection_fd, request_t *request)
{
    // Fetch from in memory map
    const char *value = kv_get(server->kv_store, request->key);
    printf("GET(%s) = %s\n", request->key, value);

    // Build a response
    response_t response = {
            .response_status = OK,
            .key = request->key,
            .value = value,
            .error_string = "",
    };

    return send_response(connection_fd, &response);
}

int handle_set_request(kv_server_t *server, int connection_fd, request_t *request)
{
    const char *old_value = kv_set(server->kv_store, request->key, request->value);
    printf("SET(%s, %s) = %s\n", request->key, request->value, old_value);

    // Build a response
    response_t response = {
            .response_status = OK,
            .key = request->key,
            .value = old_value,
            .error_string = "",
    };

    return send_response(connection_fd, &response);
}

int handler(kv_server_t *server, int connection_fd)
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

    switch (request->op_type) {
    case GET:
        return handle_get_request(server, connection_fd, request);
    case SET:
        return handle_set_request(server, connection_fd, request);
    default:
        perror("unsupported operation\n");
        goto error;
    }

error:
    free(request);
    free(request_buffer);
    return -1;
}


/**
 * SERVER ENTRYPOINT
 */

int main(int argc, char *argv[])
{
    // Allocate the server and start it
    kv_server_t *server = create_server();
    if(listen_and_serve(server)) {
        perror("server error");
        exit(EXIT_FAILURE);
    }

    free_server(server);
    return 0;
}

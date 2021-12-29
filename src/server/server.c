#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include "server.h"

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
        return -1;
    }

    // Listen for connections
    if (listen(socket_fd, MAX_TCP_QUEUE)) {
        perror("error marking socket as passive");
        return -1;
    }

    // Accept connections in loop
    while (1) {
        if ((conn_fd = accept(socket_fd, (struct sockaddr *)&socket_address, &socket_length)) == -1) {
            perror("error awaiting socket connections");
            return -1;
        }

        // Forward the request to the handler
        handler(conn_fd);
    }

    return 0;
}

int handler(int connection_fd)
{
    printf("Got conection with file descriptor: %d\n", connection_fd);
}
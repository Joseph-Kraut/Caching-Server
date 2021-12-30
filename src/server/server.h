//
// Created by Joey Kraut on 12/29/21.
//

#ifndef CACHING_SERVER_SERVER_H
#define CACHING_SERVER_SERVER_H

#ifndef PORT
#define PORT 8000
#endif //PORT

#ifndef MAX_TCP_QUEUE
#define MAX_TCP_QUEUE 100
#endif //MAX_TCP_QUEUE

#ifndef MAX_REQUEST_LENGTH
#define MAX_REQUEST_LENGTH 4096
#endif //MAX_REQUEST_LENGTH

// Interface
int start_tcp_server();
int handler(int);

#endif //CACHING_SERVER_SERVER_H



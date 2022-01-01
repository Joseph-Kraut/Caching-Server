cmake_minimum_required(VERSION 3.10)

project(Caching-Server C)

# Server executable
set(
        SERVER_SOURCES
        src/server/server.c
        src/server/server_utils.c
        src/server/kv_store.c
        src/server/thread_pool.c
        hashmap/hashmap.c
        include/types.h
)

add_executable(server ${SERVER_SOURCES})
target_include_directories(
        server PRIVATE
        include
        hashmap
)

# Client executable
set(
        CLIENT_SOURCES
        src/client/client.c
        include/types.h
)

add_executable(client ${CLIENT_SOURCES})
target_include_directories(client PRIVATE include)

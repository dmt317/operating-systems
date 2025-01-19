#ifndef SERVER_H
#define SERVER_H

#ifdef _WIN32
    #include <windows.h>
    #include <winsock2.h>
    typedef int socklen_t;
#else
    #include <unistd.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
#endif

void handle_client(int client_socket);

#ifdef _WIN32
    DWORD WINAPI start_server(LPVOID arg);
#else
    void* start_server(void *arg);
#endif

#endif // SERVER_H
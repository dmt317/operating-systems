#ifndef SERVER_H
#define SERVER_H

void handle_client(int client_socket);

#ifdef _WIN32
    DWORD WINAPI start_server(LPVOID arg);
#else
    void* start_server(void *arg);
#endif

#endif // SERVER_H
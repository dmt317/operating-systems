#ifndef SERVER_H
#define SERVER_H

void handle_client(int client_socket);

void* start_server(void *arg);

#endif // SERVER_H
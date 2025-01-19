#ifndef HANDLERS_H
#define HANDLERS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#include <sqlite3.h>
#ifdef _WIN32
    #include <winsock2.h>
#else
    #include <sys/socket.h>
#endif

void send_response(int client_socket, const char *status, const char *response);

void handle_404(int client_socket);

void handle_400(int client_socket);

void handle_average_temperature(int client_socket, sqlite3 *db, const char *table_name);

void handle_current_temperature(int client_socket, sqlite3 *db);

#endif // HANDLERS_H
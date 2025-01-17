#ifndef HANDLERS_H
#define HANDLERS_H

#include <sqlite3.h>

void send_response(int client_socket, const char *status, const char *response);

void handle_404(int client_socket);

void handle_400(int client_socket);

void handle_average_temperature(int client_socket, sqlite3 *db, const char *table_name);

void handle_current_temperature(int client_socket, sqlite3 *db);

#endif // HANDLERS_H
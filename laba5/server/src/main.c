#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"
#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <signal.h>
    #include <arpa/inet.h>
    #include <sys/socket.h>
#endif

#include "utils.h"
#include "database.h"

#define PORT 8080
#define BUFFER_SIZE 1024
#define DB_PATH "../database/temperature.db"

volatile int running = 1;

#if _WIN32
BOOL WINAPI signal_handler(DWORD signal) {
    if (signal == CTRL_C_EVENT) {
        running = 0;
        return TRUE;
    }
    return FALSE;
}
#else
void signal_handler(int signum) {
    running = 0;
}
#endif

void send_response(int client_socket, const char *status, const char *response) {
    char headers[BUFFER_SIZE];
    int header_len = snprintf(headers, sizeof(headers),
                          "HTTP/1.1 %s\r\n"
                          "Content-Type: application/json\r\n"
                          "Access-Control-Allow-Origin: *\r\n"
                          "Content-Length: %zu\r\n"
                          "\r\n",
                          status, strlen(response));

    if (header_len < 0 || header_len >= BUFFER_SIZE) {
        fprintf(stderr, "Error formatting HTTP headers\n");
        return;
    }

    if (send(client_socket, headers, header_len, 0) < 0) {
        perror("Error sending HTTP headers");
        return;
    }

    if (send(client_socket, response, strlen(response), 0) < 0) {
        perror("Error sending HTTP body");
    }
}

void handle_404(int client_socket) {
    cJSON *json_response = cJSON_CreateObject();
    cJSON_AddStringToObject(json_response, "error", "404 Not Found");
    cJSON_AddStringToObject(json_response, "message", "The requested page was not found.");

    char *response_string = cJSON_PrintUnformatted(json_response);
    send_response(client_socket, "404 Not Found", response_string);

    cJSON_Delete(json_response);
    free(response_string);
}

void handle_400(int client_socket) {
    cJSON *json_response = cJSON_CreateObject();
    cJSON_AddStringToObject(json_response, "error", "400 Bad Request");
    cJSON_AddStringToObject(json_response, "message", "Your request is invalid. Check the syntax.");

    char *response_string = cJSON_PrintUnformatted(json_response);
    send_response(client_socket, "400 Bad Request", response_string);

    cJSON_Delete(json_response);
    free(response_string);
}

void handle_average_temperature(int client_socket, sqlite3 *db, const char *table_name) {
    sqlite3_stmt *stmt;
    char query[128];
    snprintf(query, sizeof(query), "SELECT timestamp, temperature FROM %s;", table_name);

    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Query preparation error: %s\n", sqlite3_errmsg(db));
        handle_400(client_socket);
        return;
    }

    cJSON *json_array = cJSON_CreateArray();

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char *timestamp = sqlite3_column_text(stmt, 0);
        double temperature = sqlite3_column_double(stmt, 1);

        cJSON *record = cJSON_CreateObject();
        cJSON_AddStringToObject(record, "timestamp", (const char *)timestamp);
        cJSON_AddNumberToObject(record, "temperature", temperature);
        cJSON_AddItemToArray(json_array, record);
    }

    sqlite3_finalize(stmt);

    char *json_string = cJSON_Print(json_array);
    send_response(client_socket, "200 OK", json_string);

    cJSON_Delete(json_array);
    free(json_string);
    close(client_socket);
}

void handle_current_temperature(int client_socket, sqlite3 *db) {
    const char *query = "SELECT timestamp, temperature FROM temperature ORDER BY timestamp DESC LIMIT 1;";
    sqlite3_stmt *stmt;

    if (sqlite3_prepare_v2(db, query, -1, &stmt, NULL) != SQLITE_OK) {
        fprintf(stderr, "Query preparation error: %s\n", sqlite3_errmsg(db));
        send_response(client_socket, "500 Internal Server Error", "{\"error\": \"Database query failed\"}");
        return;
    }

    cJSON *json_response = cJSON_CreateObject();

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        const unsigned char *timestamp = sqlite3_column_text(stmt, 0);
        double temperature = sqlite3_column_double(stmt, 1);

        cJSON_AddStringToObject(json_response, "timestamp", (const char *)timestamp);
        cJSON_AddNumberToObject(json_response, "temperature", temperature);
    } else {
        cJSON_AddStringToObject(json_response, "error", "No data available");
    }

    char *response_string = cJSON_PrintUnformatted(json_response);
    send_response(client_socket, "200 OK", response_string);

    sqlite3_finalize(stmt);
    cJSON_Delete(json_response);
    free(response_string);
}

void handle_client(int client_socket) {
    sqlite3 *db;
    char *err_msg = 0;

    int rc = sqlite3_open(DB_PATH, &db);
    if (rc != SQLITE_OK) {
        fprintf(stderr, "Error opening database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }

    char buffer[BUFFER_SIZE];
    read(client_socket, buffer, BUFFER_SIZE);

    printf("Request received: %s\n", buffer);

    char *method = strtok(buffer, " ");
    char *route = strtok(NULL, " ");

    if (method == NULL || route == NULL || strcmp(method, "GET") != 0) {
        handle_400(client_socket);
        return;
    }

    printf("Method: %s, Route: %s\n", method, route);

    if (strcmp(route, "/api/current_temperature") == 0) {
        handle_current_temperature(client_socket, db);
    } else if (strcmp(route, "/api/average_temperature/hour") == 0) {
        handle_average_temperature(client_socket, db, "avg_temperature_hour");
    } else if (strcmp(route, "/api/average_temperature/day") == 0) {
        handle_average_temperature(client_socket, db, "avg_temperature_day");
    } else {
        handle_404(client_socket);
    }

    sqlite3_close(db);
}

int main() {
    #if _WIN32
        if (!SetConsoleCtrlHandler(signal_handler, TRUE)) {
            fprintf(stderr, "Error setting signal handler.\n");
            return EXIT_FAILURE;
        }
    #else
        struct sigaction end_action;
        end_action.sa_handler = signal_handler;
        sigemptyset(&end_action.sa_mask);
        end_action.sa_flags = 0;
        if (sigaction(SIGINT, &end_action, NULL) == -1) {
            perror("Error setting signal handler");
            exit(EXIT_FAILURE);
        }
    #endif

    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 10) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    printf("Server is running at http://localhost:%d\n", PORT);

    while (running) {
        client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        handle_client(client_socket);

        close(client_socket);
    }

    close(server_socket);
    return 0;
}

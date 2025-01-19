#include "handlers.h"

#define BUFFER_SIZE 1024

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
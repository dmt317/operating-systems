#include "server.h"
#include "handlers.h"
#include "globals.h"

#define PORT 8080
#define BUFFER_SIZE 1024
#define DB_PATH "../database/temperature.db"

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
    #ifdef _WIN32
        recv(client_socket, buffer, BUFFER_SIZE, 0);
    #else
        read(client_socket, buffer, BUFFER_SIZE);
    #endif

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

#ifdef _WIN32
    DWORD WINAPI start_server(LPVOID arg)
#else
    void* start_server(void *arg)
#endif
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    #ifdef _WIN32
        WSADATA wsa;
        if (WSAStartup(MAKEWORD(2, 2), &wsa) != 0) {
            fprintf(stderr, "WSAStartup failed. Error Code: %d\n", WSAGetLastError());
            exit(EXIT_FAILURE);
        }
    #endif

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0) {
        perror("Socket creation failed");
        #ifdef _WIN32
            WSACleanup();
        #endif
        exit(EXIT_FAILURE);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        #ifdef _WIN32
            closesocket(server_socket);
            WSACleanup();
        #else
            close(server_socket);
        #endif
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 10) < 0) {
        perror("Listen failed");
        #ifdef _WIN32
            closesocket(server_socket);
            WSACleanup();
        #else
            close(server_socket);
        #endif
        exit(EXIT_FAILURE);
    }

    printf("Server is running at http://localhost:%d\n", PORT);

    while (running) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(server_socket, &read_fds);

        struct timeval timeout = {1, 0};
        int activity = select(server_socket + 1, &read_fds, NULL, NULL, &timeout);

        if (activity < 0 && running) {
            perror("Select failed");
            break;
        }

        if (activity > 0 && FD_ISSET(server_socket, &read_fds)) {
            client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
            if (client_socket < 0) {
                perror("Accept failed");
                continue;
            }

            handle_client(client_socket);
            #ifdef _WIN32
                closesocket(client_socket);
            #else
                close(client_socket);
            #endif
        }
    }

    #ifdef _WIN32
        closesocket(server_socket);
        WSACleanup();
    #else
        close(server_socket);
    #endif

    return NULL;
}
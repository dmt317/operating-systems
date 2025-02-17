#ifdef _WIN32
    #include <windows.h>
#else
    #include <pthread.h>
#endif

#include "serialport_reader.h"
#include "utils.h"
#include "database.h"
#include "server.h"
#include "globals.h"

#define SLEEP_INTERVAL 1
#define HOUR 3600
#define DAY 86400

volatile sig_atomic_t running = 1;

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

    sqlite3 *db = init_database();

    #ifdef _WIN32
        HANDLE server_thread;
        DWORD thread_id;

        server_thread = CreateThread(NULL, 0, start_server, NULL, 0, &thread_id);

        if (server_thread == NULL) {
            printf("Failed to create server thread. Error: %lu\n", GetLastError());
            return -1;
        }
    #else
        pthread_t server_thread;
        if (pthread_create(&server_thread, NULL, start_server, NULL) != 0) {
             perror("Failed to create server thread");
             return 1;
        }
    #endif

    double temp;
    double avg_temp_hour = 0.0;
    double avg_temp_day = 0.0;

    time_t start_time = time(NULL);
    struct tm *local_time = localtime(&start_time);

    #if _WIN32
        HANDLE fd = init_port_windows();
    #else
        int fd = init_port_posix();
    #endif

    if (fd != -1) {
        while (running) {
            #if _WIN32
                temp = read_from_port_windows(fd);
            #else
                temp = read_from_port_posix(fd);
            #endif

            if (!running) break;

            char timestamp[64];
            time_t current_time = time(NULL);
            get_timestamp(timestamp, sizeof(timestamp), current_time);
            insert_db(db, TEMPERATURE_TABLE, timestamp, temp);

            avg_temp_hour += temp;
            if (((current_time - start_time) % HOUR == 0) && (current_time != start_time)) {
                avg_temp_hour /= HOUR;
                insert_db(db, AVG_TEMPERATURE_HOUR_TABLE, timestamp, avg_temp_hour);
                avg_temp_day += avg_temp_hour;
            }

            if (((current_time - start_time) % DAY == 0) && (current_time != start_time)) {
                avg_temp_day /= 24;
                timestamp[10] = '\0';
                insert_db(db, AVG_TEMPERATURE_DAY_TABLE, timestamp, avg_temp_day);
            }
        }
    }

    sqlite3_close(db);

    printf("Stopping server...\n");

    #ifdef _WIN32
        WaitForSingleObject(server_thread, INFINITE);

        CloseHandle(server_thread);
    #else
        pthread_join(server_thread, NULL);
    #endif

    printf("Server stopped. Exiting program.\n");
    return 0;
}

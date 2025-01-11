#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <signal.h>
#endif

#include "../../include/serialport_reader.h"
#include "../../include/utils.h"

#if _WIN32
    #define TEMPERATURE_LOG_PATH "../logs/temperature.log"
    #define HOURLY_LOG_PATH "../logs/avg_temperature_hour.log"
    #define DAILY_LOG_PATH "../logs/avg_temperature_day.log"
#else
    #define TEMPERATURE_LOG_PATH "../temperature_logger/logs/temperature.log"
    #define HOURLY_LOG_PATH "../temperature_logger/logs/avg_temperature_hour.log"
    #define DAILY_LOG_PATH "../temperature_logger/logs/avg_temperature_day.log"
#endif
#define SLEEP_INTERVAL 1
#define HOUR 3600
#define DAY 86400

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

    float temp;
    float avg_temp_hour = 0.0;
    float avg_temp_day = 0.0;

    int day_count = 0;

    time_t start_time = time(NULL);
    struct tm *local_time = localtime(&start_time);
    int month = local_time->tm_mon + 1;
    int year = local_time->tm_year + 1900;

    #if _WIN32
        HANDLE fd = init_port_windows();
    #else
        int fd = init_port_posix();
    #endif

    printf("Starting temperature logger\n");

    if (fd != -1) {
        while (running) {
            #if _WIN32
                temp = read_from_port_windows(fd);
            #else
                temp = read_from_port_posix(fd);
            #endif

            if (!running) break;
            printf("Read: %.2f\n", temp);

            char timestamp[64];
            time_t current_time = time(NULL);
            get_timestamp(timestamp, sizeof(timestamp), current_time);

            char log_message[128];
            snprintf(log_message, sizeof(log_message), "[%s] Temperature: %.2f", timestamp, temp);
            write_log(TEMPERATURE_LOG_PATH, log_message);

            avg_temp_hour += temp;
            if (((current_time - start_time) % HOUR == 0) && (current_time != start_time)) {
                avg_temp_hour /= HOUR;
                printf("Average hourly: %.2f\n", avg_temp_hour);

                char hourly_log_message[128];
                snprintf(hourly_log_message, sizeof(hourly_log_message), "[%s] Average hourly: %.2f", timestamp, avg_temp_hour);
                write_log(HOURLY_LOG_PATH, hourly_log_message);

                avg_temp_day += avg_temp_hour;
            }

            if (((current_time - start_time) % DAY == 0) && (current_time != start_time)) {
                day_count++;
                avg_temp_day /= DAY;
                printf("Average daily: %.2f\n", avg_temp_day);

                char daily_log_message[128];
                snprintf(daily_log_message, sizeof(daily_log_message), "[%s] Average daily: %.2f", timestamp, avg_temp_day);
                write_log(DAILY_LOG_PATH, daily_log_message);

            }

            if ((current_time - start_time) > DAY) {
                remove_oldest_record(TEMPERATURE_LOG_PATH);
            }

            if ((day_count > days_in_month(month, year)) && day_count != 0) {
                remove_oldest_record(HOURLY_LOG_PATH);
            }

            if ((day_count > (is_leap_year(year) ? 366 : 365)) && day_count != 0) {
                remove_oldest_record(DAILY_LOG_PATH);
            }
        }
    }

    #if _WIN32
        CloseHandle(fd);
    #else
        close(fd);
    #endif

    printf("Temperature logger ended\n");

    return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <signal.h>

#include "../../include/serialport_reader.h"
#include "../../include/utils.h"

#define TEMPERATURE_LOG_PATH "../temperature_logger/logs/temperature.log"
#define HOURLY_LOG_PATH "../temperature_logger/logs/avg_temperature_hour.log"
#define DAILY_LOG_PATH "../temperature_logger/logs/avg_temperature_day.log"
#define SLEEP_INTERVAL 1
#define HOUR 3600
#define DAY 86400

volatile int running = 1;

void signal_handler(int signum) {
    running = 0;
}

int main() {
    struct sigaction end_action;
    end_action.sa_handler = signal_handler;
    sigemptyset(&end_action.sa_mask);
    end_action.sa_flags = 0;
    if (sigaction(SIGINT, &end_action, NULL) == -1) {
        perror("Error setting signal handler");
        exit(EXIT_FAILURE);
    }

    float temp;
    float avg_temp_hour = 0.0;
    float avg_temp_day = 0.0;

    int day_count = 0;

    time_t start_time = time(NULL);
    struct tm *local_time = localtime(&start_time);
    int month = local_time->tm_mon + 1;
    int year = local_time->tm_year + 1900;

    int fd = init_port_posix();

    printf("Starting temperature logger\n");

    if (fd != -1) {
        while (running) {
            temp = read_from_port_posix(fd);
            if (!running) break;
            printf("Read: %.2f\n", temp);

            char timestamp[64];
            time_t current_time = time(NULL);
            strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", localtime(&current_time));

            char log_message[128];
            snprintf(log_message, sizeof(log_message), "[%s] Temperature: %.2f", timestamp, temp);
            write_log(TEMPERATURE_LOG_PATH, log_message);

            avg_temp_hour += temp;
            if (((current_time - start_time) % HOUR == 0) && (current_time != start_time)) {
                avg_temp_hour /= HOUR;

                char hourly_log_message[128];
                snprintf(hourly_log_message, sizeof(hourly_log_message), "[%s] Average hourly: %.2f", timestamp, avg_temp_hour);
                write_log(HOURLY_LOG_PATH, hourly_log_message);

                avg_temp_day += avg_temp_hour;
            }

            if (((current_time - start_time) % DAY == 0) && (current_time != start_time)) {
                day_count++;
                avg_temp_day /= DAY;

                char daily_log_message[128];
                snprintf(daily_log_message, sizeof(daily_log_message), "[%s] Average daily: %.2f", timestamp, avg_temp_hour);
                write_log(DAILY_LOG_PATH, daily_log_message);

                cleanup_log_file(TEMPERATURE_LOG_PATH);
            }

            if ((day_count % days_in_month(month, year)) == 0) {
                cleanup_log_file(HOURLY_LOG_PATH);
            }

            if ((day_count % (is_leap_year(year) ? 366 : 365)) == 0) {
                cleanup_log_file(DAILY_LOG_PATH);
            }
        }
    }

    close(fd);

    printf("Temperature logger ended\n");

    return 0;
}

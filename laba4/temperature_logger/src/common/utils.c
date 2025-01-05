#include <stdio.h>

#include "../../include/utils.h"

int is_leap_year(int year) {
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        return 1;
    }
    return 0;
}

int days_in_month(int month, int year) {
    int days_in_months[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

    if (is_leap_year(year) && month == 2) {
        return 29;
    }

    return days_in_months[month - 1];
}

void cleanup_log_file(const char *log_file_path) {
    FILE *log_file = fopen(log_file_path, "w");
    if (log_file == NULL) {
        perror("Error opening file");
        fclose(log_file);
        return;
    }

    fclose(log_file);
}

void write_log(const char *log_file_path, const char *message) {
    FILE *log_file = fopen(log_file_path, "a");
    if (log_file == NULL) {
        fprintf(stderr, "Error opening log file %s\n", log_file_path);
        return;
    }

    if (fprintf(log_file, "%s\n", message) < 0) {
        perror("Error writing to log file");
    }

    if (fclose(log_file) != 0) {
        perror("Error closing log file");
    }
}
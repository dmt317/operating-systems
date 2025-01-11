#include <stdio.h>

#include "../../include/utils.h"

#define MAX_LINE_LENGTH 256

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

void remove_oldest_record(const char *log_file_path) {
    FILE *log_file = fopen(log_file_path, "r");
    if (log_file == NULL) {
        perror("Error opening file");
        fclose(log_file);
        return;
    }

    FILE *temp_file = fopen("temp.txt", "w");
    if (!temp_file) {
        perror("Error creating temp.txt");
        fclose(temp_file);
        return;
    }

    char line[MAX_LINE_LENGTH];
    int is_first_line = 1;

    while (fgets(line, sizeof(line), log_file)) {
        if (is_first_line) {
            is_first_line = 0;
            continue;
        }
        fputs(line, temp_file);
    }

    fclose(log_file);
    fclose(temp_file);

    if (remove(log_file_path) != 0) {
        perror("Error deleting log_file");
        return;
    }
    if (rename("temp.txt", log_file_path) != 0) {
        perror("Error renaming temp.txt");
        return;
    }
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
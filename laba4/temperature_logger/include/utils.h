#ifndef UTILS_H

#define UTILS_H

int is_leap_year(int year);

int days_in_month(int month, int year);

void cleanup_log_file(const char *log_file_path);

void write_log(const char *log_file_path, const char *message);

#endif // UTILS_H
#ifndef UTILS_H

#define UTILS_H

#include <stdio.h>
#include <time.h>

void get_timestamp(char *timestamp, size_t size, time_t current_time);

int is_leap_year(int year);

int days_in_month(int month, int year);

#endif // UTILS_H
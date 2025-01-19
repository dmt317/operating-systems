#include <stdlib.h>
#ifdef _WIN32
    #include <windows.h>
#endif

#include "utils.h"

#define MAX_LINE_LENGTH 256

void get_timestamp(char *timestamp, size_t size, time_t current_time) {
    #ifdef _WIN32
        SYSTEMTIME st;
        GetLocalTime(&st);

        snprintf(timestamp, size, "%04d-%02d-%02d %02d:%02d:%02d",
                 st.wYear, st.wMonth, st.wDay,
                 st.wHour, st.wMinute, st.wSecond);
    #else
        struct tm *local_time = localtime(&current_time);
        if (local_time != NULL) {
            strftime(timestamp, size, "%Y-%m-%d %H:%M:%S", local_time);
        } else {
            snprintf(timestamp, size, "Error: Invalid time");
        }
    #endif
}
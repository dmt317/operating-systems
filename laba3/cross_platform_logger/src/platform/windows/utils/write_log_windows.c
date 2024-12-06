#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <windows.h>

#include "../../../../include/utils/write_log.h"

#define LOG_FILE_PATH "../logs/log.txt"

void write_log_windows(const char* message) {
    char buffer[256];
    
    SYSTEMTIME st;
    GetLocalTime(&st);
    
    int n = snprintf(buffer, 
                     sizeof(buffer), 
                     "%s PID=%d, time=%04d-%02d-%02d %02d:%02d:%02d.%03d\n",
                     message,
                     GetCurrentProcessId(),
                     st.wYear, 
                     st.wMonth, 
                     st.wDay,
                     st.wHour, 
                     st.wMinute, 
                     st.wSecond,
                     st.wMilliseconds);
    
    HANDLE hFile = CreateFile(LOG_FILE_PATH, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
    
    if (hFile == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        fprintf(stderr, "Failed to open log file. Error code: %lu\n", error);
        return;
    }

    DWORD bytesWritten;
    if (!WriteFile(hFile, buffer, n, &bytesWritten, NULL)) {
        DWORD error = GetLastError();
        fprintf(stderr, "Failed to write to log file. Error code: %lu\n", error);
    }
    
    CloseHandle(hFile);
}
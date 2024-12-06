#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <tchar.h>

#include "../../../../include/threads/run_copies.h"
#include "../../../../include/utils/write_log.h"
#include "../../../../include/utils/mutex.h"
#include "../../../../include/shared_memory/shared_memory.h"

#define LOG_FILE_PATH "../cross_platform_logger/logs/log.txt"

extern int stop;

DWORD WINAPI copy1_function_windows(LPVOID lpParam) {
    char log_msg[64];
    
    snprintf(log_msg, sizeof(log_msg), "Child1 started:");
    write_log(log_msg);
    
    WaitForSingleObject(hCounterMutex, INFINITE);
    *counter += 10;
    ReleaseMutex(hCounterMutex);
    
    snprintf(log_msg, sizeof(log_msg), "Child1 finished:");
    write_log(log_msg);

    return 0;
}

DWORD WINAPI copy2_function_windows(LPVOID lpParam) {
    char log_msg[64];
    
    snprintf(log_msg, sizeof(log_msg), "Child2 started:");
    write_log(log_msg);
    
    WaitForSingleObject(hCounterMutex, INFINITE);
    *counter = *counter << 1;
    ReleaseMutex(hCounterMutex);
    
    Sleep(2000);
    
    WaitForSingleObject(hCounterMutex, INFINITE);
    *counter = *counter >> 1;
    ReleaseMutex(hCounterMutex);
    
    snprintf(log_msg, sizeof(log_msg), "Child2 finished:");
    write_log(log_msg);

    return 0;
}

DWORD WINAPI run_copies_windows(LPVOID arg) {
    HANDLE thread1 = NULL, thread2 = NULL;
    DWORD thread1_id, thread2_id;

    char error_msg[128];
    int n;

    while (1) {
        WaitForSingleObject(hStopMutex, INFINITE);
        if (stop) {
            ReleaseMutex(hStopMutex);
            return 0;
        }
        ReleaseMutex(hStopMutex);

        Sleep(3000);

        if (thread1 != NULL && WaitForSingleObject(thread1, 0) == WAIT_TIMEOUT) {
            n = snprintf(error_msg, sizeof(error_msg), 
                         "One of the copies (Thread 1) is still running.\n");
            write_log(error_msg);
            continue;
        }

        if (thread2 != NULL && WaitForSingleObject(thread2, 0) == WAIT_TIMEOUT) {
            n = snprintf(error_msg, sizeof(error_msg), 
                         "One of the copies (Thread 2) is still running.\n");
            write_log(error_msg);
            continue;
        }

        thread1 = CreateThread(NULL, 0, copy1_function_windows, NULL, 0, &thread1_id);
        if (thread1 == NULL) {
            perror("Failed to create copy 1");
            exit(1);
        }

        thread2 = CreateThread(NULL, 0, copy2_function_windows, NULL, 0, &thread1_id);
        if (thread2 == NULL) {
            perror("Failed to create copy 2");
            CloseHandle(thread1);
            exit(1);
        }

        WaitForSingleObject(thread1, INFINITE);
        WaitForSingleObject(thread2, INFINITE);

        CloseHandle(thread1);
        CloseHandle(thread2);
    }

    return 0;
}

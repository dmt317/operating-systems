#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../../../include/threads/log_status.h"
#include "../../../../include/utils/write_log.h"
#include "../../../../include/utils/mutex.h"
#include "../../../../include/shared_memory/shared_memory.h"

extern int stop;            

DWORD WINAPI log_status_windows(LPVOID arg) {
    while (1) {
        WaitForSingleObject(hStopMutex, INFINITE);
        if (stop) {
            ReleaseMutex(hStopMutex);
            return 0;
        }
        ReleaseMutex(hStopMutex);

        Sleep(1000);

        char log_msg[64];
        WaitForSingleObject(hCounterMutex, INFINITE);
        snprintf(log_msg, sizeof(log_msg), "Counter=%d, ", *counter);
        ReleaseMutex(hCounterMutex);

        write_log(log_msg);
    }

    return 0;
}

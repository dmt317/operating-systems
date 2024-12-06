#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../../../include/threads/increment.h"
#include "../../../../include/shared_memory/shared_memory.h"
#include "../../../../include/utils/mutex.h"

extern int stop;

DWORD WINAPI increment_windows(LPVOID arg) {
    while (1) {
        WaitForSingleObject(hStopMutex, INFINITE);
        if (stop) {
            ReleaseMutex(hStopMutex);
            return 0;
        }
        ReleaseMutex(hStopMutex);

        Sleep(300);

        WaitForSingleObject(hCounterMutex, INFINITE);
        (*counter)++;
        ReleaseMutex(hCounterMutex);
    }

    return 0;
}

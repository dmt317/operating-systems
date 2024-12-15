#include <stdio.h>
#include <stdlib.h>

#include "../../../../include/utils/mutex.h"

HANDLE hCounterMutex = NULL;
HANDLE hStopMutex = NULL;
HANDLE hConsoleMutex = NULL;

void initialize_mutex_windows(void) {
    hCounterMutex = CreateMutex(NULL, FALSE, NULL);
    hStopMutex = CreateMutex(NULL, FALSE, NULL);
    hConsoleMutex = CreateMutex(NULL, FALSE, NULL);

    if (hCounterMutex == NULL) {
        if (hCounterMutex == NULL) {
            fprintf(stderr, "Counter mutex creation error: %lu\n", GetLastError());
            exit(EXIT_FAILURE);
        }
    }

    if (hStopMutex == NULL) {
        if (hStopMutex == NULL) {
            fprintf(stderr, "Counter mutex creation error: %lu\n", GetLastError());
            exit(EXIT_FAILURE);
        }
    }

    if (hConsoleMutex == NULL) {
        if (hConsoleMutex == NULL) {
            fprintf(stderr, "Counter mutex creation error: %lu\n", GetLastError());
            exit(EXIT_FAILURE);
        }
    }
}

void cleanup_mutex_windows(void) {
    if (hCounterMutex != NULL) {
        CloseHandle(hCounterMutex);
        hCounterMutex = NULL;
    }

    if (hStopMutex != NULL) {
        CloseHandle(hStopMutex);
        hStopMutex = NULL;
    }

    if (hConsoleMutex != NULL) {
        CloseHandle(hConsoleMutex);
        hConsoleMutex = NULL;
    }
}
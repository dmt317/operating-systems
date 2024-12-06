#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../../../include/threads/threads.h"
#include "../../../../include/utils/mutex.h"

#define LEADER_THREADS 4
#define PARTICIPANT_THREADS 2

extern DWORD WINAPI increment_windows(LPVOID arg);
extern DWORD WINAPI input_windows(LPVOID arg);
extern DWORD WINAPI log_status_windows(LPVOID arg);
extern DWORD WINAPI run_copies_windows(LPVOID arg);

typedef DWORD(WINAPI* ThreadFunc)(LPVOID);

HANDLE* create_threads_windows(int is_leader) {
    int num_threads = is_leader ? LEADER_THREADS : PARTICIPANT_THREADS;
    HANDLE* threads = malloc(num_threads * sizeof(HANDLE));
    if (!threads) {
        fprintf(stderr, "Failed to allocate memory for threads\n");
        return 0;
    }

    ThreadFunc leader_functions[LEADER_THREADS] = {
        increment_windows,
        input_windows,
        log_status_windows,
        run_copies_windows
    };

    ThreadFunc participant_functions[PARTICIPANT_THREADS] = {
        increment_windows,
        input_windows
    };

    ThreadFunc* functions = is_leader ? leader_functions : participant_functions;

    for (int i = 0; i < num_threads; i++) {
        threads[i] = CreateThread(NULL, 0, functions[i], NULL, 0, NULL);

        if (threads[i] == NULL) {
            fprintf(stderr, "Failed to create thread %d\n", i);
            for (int j = 0; j < i; j++) {
                WaitForSingleObject(threads[j], INFINITE);
                CloseHandle(threads[j]);
            }
            free(threads);
            return 0;
        }
    }

    return threads;
}

void stop_threads_windows(HANDLE* threads, int is_leader) {
    int num_threads = is_leader ? LEADER_THREADS : PARTICIPANT_THREADS;

    for (int i = 0; i < num_threads; i++) {
        if (WaitForSingleObject(threads[i], INFINITE) == WAIT_FAILED) {
            fprintf(stderr, "Failed to wait for thread %d\n", i);
        }

        if (!CloseHandle(threads[i])) {
            fprintf(stderr, "Failed to close thread handle %d\n", i);
        }
    }

    printf("All threads stopped for %s.\n", is_leader ? "leader" : "participant");

    cleanup_mutex();
}

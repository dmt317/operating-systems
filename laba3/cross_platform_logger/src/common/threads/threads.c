#include <stdio.h>
#include <stdlib.h>

#include "../../../include/threads/threads.h"

#define LEADER_THREADS 4
#define PARTICIPANT_THREADS 2

#ifdef _WIN32
    #include <windows.h>
    HANDLE* threads = NULL; 
#else
    pthread_t* threads = NULL;
#endif

void create_threads(int is_leader) {
    int num_threads = is_leader ? LEADER_THREADS : PARTICIPANT_THREADS;
    
    #ifdef _WIN32
        threads = create_threads_windows(is_leader);
        if (threads == NULL) {
            fprintf(stderr, "Failed to create threads on Windows.\n");
            return;
        }
    #else
        threads = create_threads_posix(is_leader);
        if (threads == NULL) {
            fprintf(stderr, "Failed to create threads.\n");
            return;
        }
    #endif
}

void stop_threads(int is_leader) {
    #ifdef _WIN32
        if (threads == NULL) {
            fprintf(stderr, "No threads to stop on Windows.\n");
            return;
        }

        stop_threads_windows(threads, is_leader);
        free(threads); 
        threads = NULL;
    #else
        if (threads == NULL) {
            fprintf(stderr, "No threads to stop.\n");
            return;
        }
    
        stop_threads_posix(threads, is_leader);
        free(threads); 
        threads = NULL;
    #endif
}
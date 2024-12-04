#include <stdio.h>
#include <stdlib.h>

#include "../../include/threads/threads.h"

#define NUM_THREADS 4

pthread_t threads[NUM_THREADS];

void create_threads(void) {
    #ifdef _WIN32
        create_threads_windows();
    #else
        pthread_t* threads_ptr = create_threads_posix();
        if (threads_ptr == NULL) {
            printf("Failed to create threads\n");
        } else {
            for (int i = 0; i < NUM_THREADS; i++) {
                threads[i] = threads_ptr[i];
            }
            free(threads_ptr); 
        }
    #endif
}

void stop_threads(void) {
    #ifdef _WIN32
        stop_threads_windows();
    #else
        stop_threads_posix(threads);
    #endif
}
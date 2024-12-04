#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "../../../../include/threads/threads.h"

#define NUM_THREADS 4

extern void* increment_posix(void* arg);
extern void* input_posix(void* arg);
extern void* log_status_posix(void* arg);
extern void* run_copies_posix(void* arg);

typedef void* (*ThreadFunc)(void*);

extern pthread_mutex_t mutex;

pthread_t* create_threads_posix(void) {
    pthread_t* threads = malloc(NUM_THREADS * sizeof(pthread_t));
    if (!threads) {
        perror("Failed to allocate memory for threads");
        return NULL;
    }
    
    ThreadFunc functions[NUM_THREADS] = {
        increment_posix,
        input_posix,
        log_status_posix,
        run_copies_posix
    };
            
    for (int i = 0; i < NUM_THREADS; i++) {
        int result = pthread_create(&threads[i], NULL, functions[i], NULL);
        if (result != 0) {
            perror("Failed to create thread");
            for (int j = 0; j < i; j++) {
                pthread_join(threads[j], NULL);
                }
            free(threads);
                
            return NULL;
            }
        }   
    return threads;
}

void stop_threads_posix(pthread_t* threads) {
    for (int i = 0; i < NUM_THREADS; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }
  
    printf("All threads stopped.\n");
    
    pthread_mutex_destroy(&mutex);
}
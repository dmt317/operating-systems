#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>

#include "../../../../include/threads/threads.h"

#define LEADER_THREADS 4
#define PARTICIPANT_THREADS 2

extern void* increment_posix(void* arg);
extern void* input_posix(void* arg);
extern void* log_status_posix(void* arg);
extern void* run_copies_posix(void* arg);

typedef void* (*ThreadFunc)(void*);

extern pthread_mutex_t mutex;

pthread_t* create_threads_posix(int is_leader) {
    int num_threads = is_leader ? LEADER_THREADS : PARTICIPANT_THREADS;
    pthread_t* threads = malloc(num_threads * sizeof(pthread_t));
    if (!threads) {
        perror("Failed to allocate memory for threads");
        return NULL;
    }
    
    ThreadFunc leader_functions[LEADER_THREADS] = {
        increment_posix,
        input_posix,
        log_status_posix,
        run_copies_posix
    };
    
    ThreadFunc participant_functions[PARTICIPANT_THREADS] = {
        increment_posix,
        input_posix
    };
    
    ThreadFunc* functions = is_leader ? leader_functions : participant_functions;
            
    for (int i = 0; i < num_threads; i++) {
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

void stop_threads_posix(pthread_t* threads, int is_leader) {
    int num_threads = is_leader ? LEADER_THREADS : PARTICIPANT_THREADS;
    
    for (int i = 0; i < is_leader; i++) {
        if (pthread_join(threads[i], NULL) != 0) {
            perror("Failed to join thread");
        }
    }
  
    printf("All threads stopped for %s.\n", is_leader ? "leader" : "participant");
    
    pthread_mutex_destroy(&mutex);
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "../../../../include/threads/increment.h"
#include "../../../../include/shared_memory/shared_memory.h"

extern pthread_mutex_t mutex;
extern pthread_mutex_t stop_mutex;
extern int stop;

void* increment_posix(void* arg) {
    while (1) {
        pthread_mutex_lock(&stop_mutex);
        if (stop) {
            pthread_mutex_unlock(&stop_mutex);
            return NULL;
        }
        pthread_mutex_unlock(&stop_mutex);
        
        usleep(300000);
        
        pthread_mutex_lock(&mutex);
        (*counter)++;
        pthread_mutex_unlock(&mutex);
    }
    
    return NULL;
}
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include "../../../../include/threads/log_status.h"
#include "../../../../include/utils/write_log.h"
#include "../../../../include/shared_memory/shared_memory.h"

extern pthread_mutex_t mutex;
extern pthread_mutex_t stop_mutex;
extern int stop;

void* log_status_posix(void* arg) {
    while (1) {
        pthread_mutex_lock(&stop_mutex);
        if (stop) {
            pthread_mutex_unlock(&stop_mutex);
            return NULL;
        }
        pthread_mutex_unlock(&stop_mutex);
        
        usleep(1000000);
        
        char log_msg[64];
        pthread_mutex_lock(&mutex);
        snprintf(log_msg, sizeof(log_msg), "Counter=%d, ", *counter);
        pthread_mutex_unlock(&mutex);
        write_log(log_msg);
    }
    
    return NULL;
}
#include "../../../../include/utils/mutex.h"

void initialize_mutex_posix(void) {
    pthread_mutex_t stop_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t console_mutex = PTHREAD_MUTEX_INITIALIZER;
}

void cleanup_mutex_posix(void) {
    pthread_mutex_destroy(&stop_mutex);
    pthread_mutex_destroy(&counter_mutex);
    pthread_mutex_destroy(&console_mutex);
}
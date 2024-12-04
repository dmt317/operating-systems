#ifndef THREADS_H

#define THREADS_H

#include <pthread.h>

void create_threads(int is_leader);
#ifdef _WIN32
    DWORD WINAPI create_threads_windows(LPVOID arg);
#else
    pthread_t* create_threads_posix(int is_leader);
#endif

void stop_threads(int is_leader);
#ifdef _WIN32
    void stop_threads_windows(void);
#else
    void stop_threads_posix(pthread_t* threads, int is_leader);
#endif


#endif // THREADS_H
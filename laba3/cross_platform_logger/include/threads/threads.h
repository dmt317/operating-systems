#ifndef THREADS_H

#define THREADS_H

#ifdef _WIN32
    #include <windows.h>
#else
    #include <pthread.h>
#endif

void create_threads(int is_leader);
#ifdef _WIN32
    HANDLE* create_threads_windows(int is_leader);
#else
    pthread_t* create_threads_posix(int is_leader);
#endif

void stop_threads(int is_leader);
#ifdef _WIN32
    void stop_threads_windows(HANDLE* threads, int is_leader);
#else
    void stop_threads_posix(pthread_t* threads, int is_leader);
#endif

#endif // THREADS_H
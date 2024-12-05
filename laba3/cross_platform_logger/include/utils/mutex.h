#ifndef MUTEX_H
#define MUTEX_H

#ifdef _WIN32
    #include <windows.h>
    extern HANDLE hCounterMutex;
    extern HANDLE hStopMutex;
#else
    #include <pthread.h>
    pthread_mutex_t counter_mutex; 
    pthread_mutex_t stop_mutex; 
#endif

void initialize_mutex(void);
#ifdef _WIN32
    void initialize_mutex_windows(void);
#else
    void initialize_mutex_posix(void);
#endif

void cleanup_mutex(void);
#ifdef _WIN32
    void cleanup_mutex_windows(void);
#else
    void cleanup_mutex_posix(void);
#endif

#endif // MUTEX_H

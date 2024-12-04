#include <stdio.h>
#include <unistd.h>

#include "../../include/threads/threads.h"
#include "../../include/utils/write_log.h"
#include "../../include/shared_memory/shared_memory.h"

#ifdef _WIN32
    CRITICAL_SECTION stop_mutex = CreateMutex(NULL, FALSE, NULL);
    CRITICAL_SECTION mutex = CreateMutex(NULL, FALSE, NULL);
#else
    pthread_mutex_t stop_mutex = PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
#endif

volatile int stop = 0;

int main() {
    char name[] = "/shared_counter";
    size_t size = sizeof(int);
    
    create_shared_memory(name, size);
    
    if (counter == NULL) {
        fprintf(stderr, "Failed to initialize shared memory.\n");
        return EXIT_FAILURE;
    }
    
    char start[] = "Programm started,";
    write_log(start);
    
    create_threads();
    
    // для демонстрации ограничиваем время работы программы на 10 секунд
    #ifdef _WIN32
        Sleep(10000);
    #else
        usleep(10000000);
    #endif
    
    pthread_mutex_lock(&stop_mutex);
    stop = 1;
    pthread_mutex_unlock(&stop_mutex);
        
    stop_threads();
    
    close_shared_memory(name, size);
    
    return 0;
}
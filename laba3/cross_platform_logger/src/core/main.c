#include <stdio.h>
#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
#endif

#include "../../include/threads/threads.h"
#include "../../include/utils/write_log.h"
#include "../../include/utils/is_leader.h"
#include "../../include/utils/mutex.h"
#include "../../include/shared_memory/shared_memory.h"

volatile int stop = 0;

int main() {
    initialize_mutex();
    
    int leader = is_leader();
    char name[] = "/shared_counter";
    size_t size = sizeof(int);
    
    if (leader) {
        create_shared_memory(name, size);
        if (counter == NULL) {
            fprintf(stderr, "Failed to initialize shared memory.\n");
            return EXIT_FAILURE;
        }
        
        char start[] = "Leader started,";
            
        write_log(start);
            
        create_threads(leader);
            
        // для демонстрации ограничиваем время работы программы на 10 секунд
        #ifdef _WIN32
            Sleep(10000);
        #else
            usleep(10000000);
        #endif
        
        pthread_mutex_lock(&stop_mutex);
        stop = 1;
        pthread_mutex_unlock(&stop_mutex);
        
        printf("Time is up!\n");
                
        stop_threads(leader);
        
        close_shared_memory(name, size);
        
        cleanup_lock_file_posix();
    }
    else {
        if (connect_to_shared_memory(name, size) == -1) {
            fprintf(stderr, "Failed to connect to shared memory.\n");
            return EXIT_FAILURE;
        }
        
        create_threads(leader);
        
        while (!stop) {
            #ifdef _WIN32
                Sleep(100);
            #else
                usleep(100000);
            #endif
        }
        
        stop_threads(leader);
        
        detach_shared_memory();
        
        printf("Participant finished.");
    }
    
    return 0;
}
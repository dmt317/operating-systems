#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <fcntl.h>

#include "../../../../include/threads/run_copies.h"
#include "../../../../include/utils/write_log.h"
#include "../../../../include/shared_memory/shared_memory.h"

#define LOG_FILE_PATH "../cross_platform_logger/logs/log.txt"

extern pthread_mutex_t mutex;
extern pthread_mutex_t stop_mutex;
extern int stop;

void copy1_function_posix(void) {
    char log_msg[64];
    
    pthread_mutex_lock(&mutex);
    snprintf(log_msg, sizeof(log_msg), "Child1 started:");
    write_log(log_msg);
    
    *counter += 10;
    
    snprintf(log_msg, sizeof(log_msg), "Child1 finished:");
    write_log(log_msg);
    pthread_mutex_unlock(&mutex);
}

void copy2_function_posix(void) {
    char log_msg[64];
    
    snprintf(log_msg, sizeof(log_msg), "Child2 started:");
    write_log(log_msg);
    
    pthread_mutex_lock(&mutex);
    *counter = *counter << 1;
    pthread_mutex_unlock(&mutex);
    
    usleep(2000000);
    
    pthread_mutex_lock(&mutex);
    *counter = *counter >> 1;
    pthread_mutex_unlock(&mutex);
    
    snprintf(log_msg, sizeof(log_msg), "Child2 finished:");
    write_log(log_msg);
}

void* run_copies_posix(void* arg) {
    pid_t pid1 = -1, pid2 = -1;
    int status1 = 0, status2 = 0;
        
    char error_msg[64];
    int n;

    while (1) {
        pthread_mutex_lock(&stop_mutex);
        if (stop) {
            pthread_mutex_unlock(&stop_mutex);
            return NULL;
        }
        pthread_mutex_unlock(&stop_mutex);
        
        usleep(3000000);
        
        int fd = open(LOG_FILE_PATH, O_RDWR | O_CREAT | O_APPEND, 0644);
        if (fd == -1) {
            perror("open");
        }
    
        if (pid1 > 0 && waitpid(pid1, &status1, WNOHANG) == 0) {
            n = snprintf(error_msg, 
                         sizeof(error_msg), 
                         "One of the copies (pid %d) is still running.\n",
                         pid1);
            write(fd, error_msg, n);
            continue;
        }
        
        if (pid2 > 0 && waitpid(pid2, &status2, WNOHANG) == 0) {
            n = snprintf(error_msg, 
                         sizeof(error_msg), 
                         "One of the copies (pid %d) is still running.\n",
                         pid2);
            write(fd, error_msg, n);
            continue;
        }
        
        close(fd);
            
        pid_t pid1 = fork();
        if (pid1 == 0) {
            copy1_function_posix();
            exit(0);
        } else if (pid1 < 0) {
            perror("Failed to create copy 1");
            exit(1);
        }
        
        pid_t pid2 = fork();
        if (pid2 == 0) {
            copy2_function_posix();
            exit(0);
        } else if (pid2 < 0) {
            perror("Failed to create copy 2");
            exit(1);
        }
        
        waitpid(pid1, &status1, 0);
        waitpid(pid2, &status2, 0);
    }
    
    return NULL;
}
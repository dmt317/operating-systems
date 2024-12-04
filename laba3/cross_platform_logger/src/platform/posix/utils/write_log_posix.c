#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <sys/time.h>
#include <unistd.h>

#include "../../../../include/utils/write_log.h"

#define LOG_FILE_PATH "../cross_platform_logger/logs/log.txt"

void write_log_posix(const char* message) {
    char buffer[256];
    
    struct timeval tv;
    gettimeofday(&tv, NULL);
    
    struct tm *tm_info;
    tm_info = localtime(&tv.tv_sec);
    
    int n = snprintf(buffer, 
                     sizeof(buffer), 
                     "%s PID=%d, time=%04d-%02d-%02d %02d:%02d:%02d.%03d\n",
                     message,
                     getpid(),
                     tm_info->tm_year + 1900, 
                     tm_info->tm_mon + 1, 
                     tm_info->tm_mday,
                     tm_info->tm_hour, 
                     tm_info->tm_min, 
                     tm_info->tm_sec,
                     tv.tv_usec / 1000);
    
    int fd = open(LOG_FILE_PATH, O_RDWR | O_CREAT | O_APPEND, 0644);
    if (fd == -1) {
        perror("open");
    }
    
    if (write(fd, buffer, n) == -1) {
        perror("write");
    }
    
    close(fd);
}
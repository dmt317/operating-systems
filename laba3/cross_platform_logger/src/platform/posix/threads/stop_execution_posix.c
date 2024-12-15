#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <sys/select.h>

#include "../../../../include/threads/stop_execution.h"
#include "../../../../include/utils/mutex.h"

extern int stop;

int console_blocked = 0;

void* stop_execution_posix(void* arg) {
    while (1) {
        pthread_mutex_lock(&stop_mutex);
        if (stop) {
            pthread_mutex_unlock(&stop_mutex);
            return NULL;
        }
        pthread_mutex_unlock(&stop_mutex);

        usleep(10000000);

        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(STDIN_FILENO, &read_fds);

        struct timeval timeout;
        timeout.tv_sec = 9;
        timeout.tv_usec = 0;

        pthread_mutex_lock(&console_mutex);
        console_blocked = 1;

        usleep(100000);
        printf("Do you want to continue? (y/n) ");
        fflush(stdout);

        int select_result = select(STDIN_FILENO + 1, &read_fds, NULL, NULL, &timeout);
        if (select_result > 0) {
            char answer;
            if (scanf("%c", &answer) == 1) {
                if (answer == 'n') {
                    pthread_mutex_lock(&stop_mutex);
                    stop = 1;
                    pthread_mutex_unlock(&stop_mutex);

                    console_blocked = 0;
                    pthread_mutex_unlock(&console_mutex);

                    return NULL;
                } else if (answer != 'y') {
                    printf("Incorrect input! Try again.\n");
                }
            }
        } else if (select_result == 0) {
            printf("\nTimeout. Continuing...\n");
        } else {
            perror("\nselect failed");
        }
        console_blocked = 0;
        pthread_mutex_unlock(&console_mutex);
    }
    return NULL;
}

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/select.h>
#include <ctype.h>
#include <string.h>

#include "../../../../include/shared_memory/shared_memory.h"

extern pthread_mutex_t mutex;
extern pthread_mutex_t stop_mutex;
extern int stop;

void* input_posix(void* arg) {
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    while (1) {
        pthread_mutex_lock(&stop_mutex);
        if (stop) {
            pthread_mutex_unlock(&stop_mutex);
            printf("Time is up! Input thread exiting...\n");
            return NULL;
        }
        pthread_mutex_unlock(&stop_mutex);

        fd_set readfds;
        struct timeval timeout;

        FD_ZERO(&readfds);
        FD_SET(STDIN_FILENO, &readfds);

        timeout.tv_sec = 1; 
        timeout.tv_usec = 0;

        int result = select(STDIN_FILENO + 1, &readfds, NULL, NULL, &timeout);

        if (result > 0 && FD_ISSET(STDIN_FILENO, &readfds)) {
            char buffer[100];
            if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
                buffer[strcspn(buffer, "\n")] = '\0';

                int is_numeric = 1;
                for (size_t i = 0; i < strlen(buffer); i++) {
                    if (!isdigit((unsigned char)buffer[i])) {
                        is_numeric = 0;
                        break;
                    }
                }

                if (!is_numeric) {
                    printf("Error: Input contains invalid characters. Please enter a valid number.\n");
                } else {
                    int input = atoi(buffer);
                    printf("You entered: %d\n", input);

                    pthread_mutex_lock(&mutex);
                    *counter = input;
                    pthread_mutex_unlock(&mutex);
                }
            } else {
                printf("Error reading input\n");
            }
        } else if (result == 0) {
            printf("\rWaiting for input...\n");
            fflush(stdout);
        } else {
            perror("\nselect failed");
        }
    }

    return NULL;
}
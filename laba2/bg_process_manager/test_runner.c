#include <stdio.h>
#include <string.h>
#include "run_bg.h"

int main() {
    char command[256];

    printf("Enter command to execute: ");
    if (fgets(command, sizeof(command), stdin) == NULL) {
        fprintf(stderr, "Command input error.\n");
        return 1;
    }

    // Удаление символа новой строки из команды
    size_t len = strlen(command);
    if (len > 0 && command[len - 1] == '\n') {
        command[len - 1] = '\0';
    }

    int exit_code = run_bg(command);
    if (exit_code == -1) {
        fprintf(stderr, "Child process launch error.\n");
    } else {
        printf("Child process ended with the code: %d\n", exit_code);
    }
    
    printf("The main process is completed.\n");

    return 0;
}

#include <stdio.h>
#include <string.h>
#include "run_bg.h"

int main() {
    char command[256];

    printf("Введите команду для выполнения: ");
    if (fgets(command, sizeof(command), stdin) == NULL) {
        fprintf(stderr, "Ошибка ввода команды.\n");
        return 1;
    }

    // Удаление символа новой строки из команды
    size_t len = strlen(command);
    if (len > 0 && command[len - 1] == '\n') {
        command[len - 1] = '\0';
    }

    int exit_code = run_bg(command);
    if (exit_code == -1) {
        fprintf(stderr, "Ошибка запуска программы.\n");
    } else {
        printf("Программа завершилась с кодом: %d\n", exit_code);
    }

    return 0;
}

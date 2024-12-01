#include "../../../include/run_bg.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int run_bg_posix(const char* cmd) {
    pid_t pid;
    int status;
    if ((pid = fork()) < 0) {
        /* ошибка */
        perror("fork");
        return -1;
    } else if (!pid) {
        /* child */
        execl("/bin/sh", "/bin/sh", "-c", cmd, NULL);
        /* ошибка */
        perror("execl");
        _exit(1);
    }
    /* parent waits until child process exits */
    if (waitpid(pid, &status, 0) == -1)
        return -1;
    
    if (WIFEXITED(status))
        return WEXITSTATUS(status);
    return -1;
}
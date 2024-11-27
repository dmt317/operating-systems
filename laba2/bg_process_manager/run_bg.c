#include "run_bg.h"
#include <stdio.h>
#ifdef _WIN32
    #include <windows.h>
#else
    #include <sys/types.h>
    #include <sys/wait.h>
    #include <unistd.h>
#endif

int run_bg(const char* cmd) {
    #ifdef _WIN32
        STARTUPINFO si;
        PROCESS_INFORMATION pi;
        
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);
        ZeroMemory(&pi, sizeof(pi));
        
        // Start the child process
        if (!CreateProcess(NULL, (char*)cmd, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
            printf("CreateProcess failed (%d).\n", GetLastError());
            return -1;
        }
        
        // Wait until child process exits
        WaitForSingleObject(pi.hProcess, INFINITE);
        
        DWORD exit_code;
        if (!GetExitCodeProcess(pi.hProcess, &exit_code)) {
            CloseHandle(pi.hProcess);
            CloseHandle(pi.hThread);
            return -1;
        }
        
        // Close process and thread handles
        CloseHandle(pi.hProcess);
        CloseHandle(pi.hThread);
        return (int)exit_code;
    #else 
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
    #endif
}
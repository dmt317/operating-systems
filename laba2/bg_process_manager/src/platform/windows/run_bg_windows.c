#include "../../../include/run_bg.h"
#include <stdio.h>
#include <windows.h>

int run_bg_windows(const char* cmd) {
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
}
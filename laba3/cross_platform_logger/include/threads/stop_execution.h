#ifndef STOP_EXECUTION_H

#define STOP_EXECUTION_H

extern int console_blocked;

#ifdef _WIN32
    #include <windows.h>
    DWORD WINAPI stop_execution_windows(LPVOID arg);
#else
    void* stop_execution_posix(void* arg);
#endif

#endif // STOP_EXECUTION_H
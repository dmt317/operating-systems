#ifndef LOG_STATUS_H

#define LOG_STATUS_H

#ifdef _WIN32
    #include <windows.h>
    DWORD WINAPI log_status_windows(LPVOID arg);
#else
    void* log_status_posix(void* arg);
#endif

#endif // LOG_STATUS_H
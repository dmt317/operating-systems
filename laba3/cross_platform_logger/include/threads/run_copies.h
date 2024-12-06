#ifndef RUN_COPIES_H

#define RUN_COPIES_H

#ifdef _WIN32
    #include <windows.h>
    DWORD WINAPI run_copies_windows(LPVOID arg);
#else
    void* run_copies_posix(void* arg);
#endif

#endif // RUN_COPIES_H
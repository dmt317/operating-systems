#ifndef INCREMENT_H

#define INCREMENT_H

#ifdef _WIN32
    DWORD WINAPI increment_windows(LPVOID arg);
#else
    void* increment_posix(void* arg);
#endif

#endif // INCREMENT_H
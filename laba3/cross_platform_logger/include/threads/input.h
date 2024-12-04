#ifndef INPUT_H

#define INPUT_H

#ifdef _WIN32
    DWORD WINAPI input_windows(LPVOID arg);
#else
    void* input_posix(void* arg);
#endif

#endif // INPUT_H
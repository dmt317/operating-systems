#ifndef RUN_BG_H

#define RUN_BG_H

int run_bg(const char* cmd);

#ifdef _WIN32
    int run_bg_windows(const char* cmd);
#else
    int run_bg_posix(const char* cmd);
#endif

#endif // RUN_BG_H
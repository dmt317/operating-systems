#include "../../include/run_bg.h"

int run_bg(const char* cmd) {
    #ifdef _WIN32
        return run_bg_windows(cmd);
    #else
        return run_bg_posix(cmd);
    #endif
}
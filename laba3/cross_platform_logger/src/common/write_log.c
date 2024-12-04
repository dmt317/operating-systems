#include "../../include/utils/write_log.h"

void write_log(const char* message) {
    #ifdef _WIN32
        write_log_windows(message);
    #else
        write_log_posix(message);
    #endif
}
#include "../../../include/utils/is_leader.h"

int is_leader(void) {
    #ifdef _WIN32
        return is_leader_windows();
    #else
        return is_leader_posix();
    #endif
}

void cleanup_lock_file(void) {
    #ifdef _WIN32
        cleanup_lock_file_windows();
    #else
        cleanup_lock_file_posix();
    #endif
}
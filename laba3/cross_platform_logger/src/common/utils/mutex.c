#include "../../../include/utils/mutex.h"

void initialize_mutex(void) {
    #ifdef _WIN32
        initialize_mutex_windows();
    #else
        initialize_mutex_posix();
    #endif
    
}void cleanup_mutex(void) {
    #ifdef _WIN32
        cleanup_mutex_windows();
    #else    
        cleanup_mutex_posix();
    #endif
}

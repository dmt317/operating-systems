#include <stdio.h>

#include "../../include/shared_memory/shared_memory.h"

int* counter = NULL;

void create_shared_memory(const char *name, size_t size) {
    #ifdef _WIN32 
        counter = create_shared_memory_windows(name, size);
    #else 
        counter = create_shared_memory_posix(name, size);
    #endif    
    
    if (counter == NULL) {
        fprintf(stderr, "Failed to create shared memory.\n");
        exit(EXIT_FAILURE);
    }
}

void close_shared_memory(const char *name, size_t size) {
    #ifdef _WIN32
        close_shared_memory_windows(counter, name, size);
    #else
        close_shared_memory_posix(counter, name, size);
    #endif
}

int connect_to_shared_memory(const char *name, size_t size) {
    #ifdef _WIN32
        return connect_to_shared_memory_windows(name, size);
    #else
        return connect_to_shared_memory_posix(name, size);
    #endif
}

void detach_shared_memory(void) {
    #ifdef _WIN32
        detach_shared_memory_windows();
    #else
        detach_shared_memory_posix(counter);
    #endif
}

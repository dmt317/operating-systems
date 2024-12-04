#ifndef SHARED_MEMORY_H

#define SHARED_MEMORY_H

#include <stdlib.h>

extern int* counter;

void create_shared_memory(const char *name, size_t size);
#ifdef _WIN32
    int* create_shared_memory_windows(const char *name, size_t size);
#else
    int* create_shared_memory_posix(const char *name, size_t size);
#endif

void close_shared_memory(const char *name, size_t size);
#ifdef _WIN32
    void close_shared_memory_windows(const char *name, size_t size);
#else
    void close_shared_memory_posix(int *counter, const char *name, size_t size);
#endif

#endif // SHARED_MEMORY_H
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "../../../../include/shared_memory/shared_memory.h"

HANDLE hSharedMemory = NULL;

int* create_shared_memory_windows(const char* name, size_t size) {
    if (size <= 0) {
        fprintf(stderr, "Error: size must be greater than 0\n");
        exit(1);
    }

    hSharedMemory = CreateFileMappingA(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, (DWORD)size, name);

    if (hSharedMemory == NULL) {
        fprintf(stderr, "CreateFileMapping failed with error %lu\n", GetLastError());
        exit(1);
    }

    counter = (int*)MapViewOfFile(hSharedMemory, FILE_MAP_ALL_ACCESS, 0, 0, size);

    if (counter == NULL) {
        fprintf(stderr, "MapViewOfFile failed with error %lu\n", GetLastError());
        CloseHandle(hSharedMemory);
        exit(1);
    }

    return counter;
}

void close_shared_memory_windows(int* counter, const char* name, size_t size) {
    if (counter) {
        if (!UnmapViewOfFile(counter)) {
            fprintf(stderr, "UnmapViewOfFile failed with error %lu\n", GetLastError());
        }
    }

    if (hSharedMemory) {
        CloseHandle(hSharedMemory);
        hSharedMemory = NULL;
    }
}

int connect_to_shared_memory_windows(const char* name, size_t size) {
    hSharedMemory = OpenFileMappingA(FILE_MAP_ALL_ACCESS, FALSE, name);

    if (hSharedMemory == NULL) {
        fprintf(stderr, "OpenFileMapping failed with error %lu\n", GetLastError());
        return -1;
    }

    counter = (int*)MapViewOfFile(hSharedMemory, FILE_MAP_ALL_ACCESS, 0, 0, size);

    if (counter == NULL) {
        fprintf(stderr, "MapViewOfFile failed with error %lu\n", GetLastError());
        CloseHandle(hSharedMemory);
        return -1;
    }

    printf("Connected to shared memory: %s\n", name);
    return 0;
}

void detach_shared_memory_windows(int* counter) {
    if (counter) {
        if (!UnmapViewOfFile(counter)) {
            fprintf(stderr, "UnmapViewOfFile failed with error %lu\n", GetLastError());
        } else {
            printf("Shared memory detached successfully.\n");
        }
    }

    if (hSharedMemory) {
        CloseHandle(hSharedMemory);
        hSharedMemory = NULL;
    }
}

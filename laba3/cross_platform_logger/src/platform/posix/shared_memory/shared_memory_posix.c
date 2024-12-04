#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <unistd.h>

#include "../../../../include/shared_memory/shared_memory.h"

int* create_shared_memory_posix(const char *name, size_t size) {
    int shm_fd;
    int* counter;
    
    shm_fd = shm_open(name, O_CREAT | O_RDWR, 0777);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(1);
    }
    
    if (size <= 0) {
        fprintf(stderr, "Error: size must be greater than 0\n");
        exit(1);
    }
    
    if (ftruncate(shm_fd, size + 1) == -1) {
        perror("ftruncate");
        exit(1);
    }
    
    counter = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (counter == MAP_FAILED) {
        perror("mmap failed");
        close(shm_fd);
        shm_unlink(name);
        exit(1);
    }
    
    // memset(shared_mem, 0, size);
    
    return counter;
}

void close_shared_memory_posix(int *counter, const char *name, size_t size) {
    if (munmap(counter, size) == -1) {
        perror("munmap");
    }
    
    if (shm_unlink(name) == -1) {
        perror("shm_unlink");
    }
}

int connect_to_shared_memory_posix(const char* name, size_t size) {
    int shm_fd = shm_open(name, O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Failed to open shared memory");
        return -1;
    }

    counter = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (counter == MAP_FAILED) {
        perror("Failed to map shared memory");
        close(shm_fd);
        return -1;
    }
    
    close(shm_fd);

    printf("Connected to shared memory: %s\n", name);
    return 0;
}

void detach_shared_memory_posix(int* counter) {
    if (counter) {
        if (munmap(counter, sizeof(int)) == -1) {
            perror("Failed to unmap shared memory");
        } else {
            printf("Shared memory detached successfully.\n");
        }
    }
}
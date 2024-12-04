#include <sys/file.h>
#include <fcntl.h>
#include <unistd.h>

#include "../../../../include/utils/is_leader.h"

int is_leader_posix() {
    int lock_fd = open("leader.lock", O_CREAT | O_EXCL, 0666);
    if (lock_fd >= 0) {
        close(lock_fd); 
        return 1;    
    }
    return 0;          
}

void cleanup_lock_file_posix() {
    unlink("leader.lock");
}
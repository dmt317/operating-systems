#include <windows.h>

#include "../../../../include/utils/is_leader.h"

HANDLE lock_file_handle = NULL;

int is_leader_windows() {
    lock_file_handle = CreateFile("leader.lock", GENERIC_WRITE, 0, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

    if (lock_file_handle == INVALID_HANDLE_VALUE) {
        if (GetLastError() == ERROR_FILE_EXISTS || GetLastError() == ERROR_ALREADY_EXISTS) {
            return 0;
        }
        return 0;
    }

    return 1;
}

void cleanup_lock_file_windows() {
    if (lock_file_handle != NULL) {
        CloseHandle(lock_file_handle);
        lock_file_handle = NULL;

        DeleteFile("leader.lock");
    }
}

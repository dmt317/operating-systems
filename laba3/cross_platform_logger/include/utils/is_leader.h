#ifndef IS_LEADER_H

#define IS_LEADER_H

int is_leader(void);
#ifdef _WIN32
    int is_leader_windows(void);
#else
    int is_leader_posix(void);
#endif

void cleanup_lock_file(void);
#ifdef _WIN32
    void cleanup_lock_file_windows(void);
#else
    void cleanup_lock_file_posix(void);
#endif

#endif // IS_LEADER_H
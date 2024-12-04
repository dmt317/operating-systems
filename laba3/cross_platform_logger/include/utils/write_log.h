#ifndef WRITE_LOG_H

#define WRITE_LOG_H

void write_log(const char* message);

#ifdef _WIN32
    void write_log_windows(const char* message);
#else
    void write_log_posix(const char* message);
#endif

#endif // WRITE_LOG_H
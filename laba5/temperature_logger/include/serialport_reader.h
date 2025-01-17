#ifndef SERIALPORT_READER_H

#define SERIALPORT_READER_H

#ifdef _WIN32
    HANDLE init_port_windows(void);

    double read_from_port_windows(HANDLE hComm);
#else
    int init_port_posix(void);

    double read_from_port_posix(int fd);
#endif

#endif // SERIALPORT_READER_H
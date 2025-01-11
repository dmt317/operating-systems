#ifndef SERIALPORT_READER_H

#define SERIALPORT_READER_H

#ifdef _WIN32
    HANDLE init_port_win(void);

    float read_from_port_posix(HANDLE hComm);
#else
    int init_port_posix(void);

    float read_from_port_posix(int fd);
#endif

#endif // SERIALPORT_READER_H
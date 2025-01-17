#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <termios.h>
#include <string.h>

#include "../../../include/serialport_reader.h"

#define PORT_NAME "/dev/ttys007"

int init_port_posix() {
    int fd = open(PORT_NAME, O_RDONLY | O_NOCTTY);
    if (fd == -1) {
        perror("Failed to open serial port");
        return -1;
    }

    struct termios options;
    if (tcgetattr(fd, &options) != 0) {
        perror("Failed to get attributes");
        close(fd);
        return -1;
    }

    cfsetispeed(&options, B9600);
    cfsetospeed(&options, B9600);

    options.c_cflag |= (CLOCAL | CREAD);
    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_oflag &= ~OPOST;

    if (tcsetattr(fd, TCSANOW, &options) != 0) {
        perror("Failed to set attributes");
        close(fd);
        return -1;
    }

    return fd;
}

float read_from_port_posix(int fd) {
    char buffer[256];
    int bytes_read = read(fd, buffer, sizeof(buffer) - 1);

    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        float result = strtof(buffer, NULL);
        return result;
    } else if (bytes_read == 0) {
        usleep(100000);
        return 0.0;
    } else {
        perror("Read error");
        return -1;
    }
}
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <fcntl.h>
    #include <termios.h>
#endif

#define PORT_NAME "COM3"
#define DEVICE_PATH "/dev/ttys000"
#define SLEEP_INTERVAL 1

float generate_temperature() {
    return (rand() % 8001 - 3000) / 100.0;
}

int init_port(const char *port_name) {
#ifdef _WIN32
    HANDLE hPort = CreateFileA(
        port_name, GENERIC_WRITE, 0, NULL,
        OPEN_EXISTING, 0, NULL
    );
    if (hPort == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Failed to open COM port: %s\n", port_name);
        return -1;
    }
    DCB dcb = {0};
    dcb.DCBlength = sizeof(dcb);
    GetCommState(hPort, &dcb);
    dcb.BaudRate = CBR_9600;
    dcb.ByteSize = 8;
    dcb.Parity = NOPARITY;
    dcb.StopBits = ONESTOPBIT;
    SetCommState(hPort, &dcb);
    return (int)hPort;
#else
    int fd = open(port_name, O_WRONLY | O_NOCTTY);
    if (fd < 0) {
        perror("Failed to open serial port");
        return -1;
    }
    struct termios tty = {0};
    if (tcgetattr(fd, &tty) != 0) {
        perror("Failed to get attributes");
        close(fd);
        return -1;
    }
    cfsetospeed(&tty, B9600);
    cfsetispeed(&tty, B9600);
    tty.c_cflag = (tty.c_cflag & ~CSIZE) | CS8;
    tty.c_cflag |= CLOCAL | CREAD;
    tty.c_cflag &= ~(PARENB | PARODD);
    tty.c_cflag &= ~CSTOPB;
    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("Failed to set attributes");
        close(fd);
        return -1;
    }
    return fd;
#endif
}

void send_temperature(int port) {
    float temperature = generate_temperature();
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%.2f\n", temperature);
    #ifdef _WIN32
        DWORD bytes_written;
        WriteFile((HANDLE)port, buffer, strlen(buffer), &bytes_written, NULL);
    #else
        write(port, buffer, strlen(buffer));
    #endif
    printf("Sent temperature: %s", buffer);
}

int main() {
    srand(time(NULL));

    const char *port_name =
    #ifdef _WIN32
        PORT_NAME;
    #else
        DEVICE_PATH;
    #endif

    int port = init_port(port_name);
    if (port < 0) {
        return EXIT_FAILURE;
    }

    printf("Device simulator started. Sending data to %s...\n", port_name);

    while (1) {
        send_temperature(port);
        #ifdef _WIN32
            Sleep(SLEEP_INTERVAL * 1000);
        #else
            sleep(SLEEP_INTERVAL);
        #endif
    }

    #ifdef _WIN32
        CloseHandle((HANDLE)port);
    #else
        close(port);
    #endif

    return EXIT_SUCCESS;
}

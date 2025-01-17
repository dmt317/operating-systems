#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <fcntl.h>
    #include <termios.h>
    #include <signal.h>
#endif

#define PORT_NAME "COM3"
#define DEVICE_PATH "/dev/ttys007"
#define SLEEP_INTERVAL 1
#define ALPHA 0.1 // Коэффициент сглаживания (0 < ALPHA ≤ 1)

volatile int running = 1;

#if _WIN32
    BOOL WINAPI signal_handler(DWORD signal) {
        if (signal == CTRL_C_EVENT) {
            printf("\nCaught signal %d. Cleaning up...\n", signal);
            running = 0;
            return TRUE;
        }
        return FALSE;
    }
#else
    void signal_handler(int signum) {
        printf("\nCaught signal %d. Cleaning up...\n", signum);
        running = 0;
    }
#endif

double generate_temperature(double min, double max) {
    return min + (rand() / (double)RAND_MAX) * (max - min);
}

double exponential_smoothing(double current_temp, double previous_smoothed) {
    return ALPHA * current_temp + (1 - ALPHA) * previous_smoothed;
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
        return 1;
    }
    struct termios options = {0};
    if (tcgetattr(fd, &options) != 0) {
        perror("Failed to get attributes");
        close(fd);
        return 1;
    }
    cfsetospeed(&options, B9600);
    cfsetispeed(&options, B9600);

    options.c_cflag = (options.c_cflag & ~CSIZE) | CS8;
    options.c_cflag |= CLOCAL | CREAD;
    options.c_cflag &= ~(PARENB | PARODD);
    options.c_cflag &= ~CSTOPB;
    if (tcsetattr(fd, TCSANOW, &options) != 0) {
        perror("Failed to set attributes");
        close(fd);
        return -1;
    }
    return fd;
#endif
}

int main() {
    srand(time(NULL));

    double smoothed_temp = 0.0;
    int first = 1;

    #if _WIN32
        if (!SetConsoleCtrlHandler(signal_handler, TRUE)) {
            fprintf(stderr, "Error setting signal handler.\n");
            return EXIT_FAILURE;
        }
    #else
        struct sigaction sa;
        sa.sa_handler = signal_handler;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        if (sigaction(SIGINT, &sa, NULL) == -1) {
            perror("Error setting signal handler");
            exit(EXIT_FAILURE);
        }
    #endif

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

    while (running) {
        double temperature = generate_temperature(-20.0, 32.0);
        if (first) {
            smoothed_temp = temperature;
            first = 0;
        } else {
            smoothed_temp = exponential_smoothing(temperature, smoothed_temp);
        }

        char buffer[32];
        snprintf(buffer, sizeof(buffer), "%.2f\n", smoothed_temp);
        #ifdef _WIN32
            DWORD bytes_written;
            WriteFile((HANDLE)port, buffer, strlen(buffer), &bytes_written, NULL);
        #else
            write(port, buffer, strlen(buffer));
        #endif

        printf("Sent temperature: %s", buffer);

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
    printf("Device simulator ended.\n");

    return EXIT_SUCCESS;
}

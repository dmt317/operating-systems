#include <windows.h>
#include <stdio.h>
#include <stdlib.h>

#include "serialport_reader.h"

#define PORT_NAME "COM4"

HANDLE init_port_windows() {
    HANDLE hComm = CreateFile(
        PORT_NAME,
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL
    );

    if (hComm == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Failed to open serial port. Error: %ld\n", GetLastError());
        return INVALID_HANDLE_VALUE;
    }

    DCB dcbSerialParams = { 0 };
    dcbSerialParams.DCBlength = sizeof(dcbSerialParams);

    if (!GetCommState(hComm, &dcbSerialParams)) {
        fprintf(stderr, "Failed to get serial port state. Error: %ld\n", GetLastError());
        CloseHandle(hComm);
        return INVALID_HANDLE_VALUE;
    }

    dcbSerialParams.BaudRate = CBR_9600;
    dcbSerialParams.ByteSize = 8;
    dcbSerialParams.Parity = NOPARITY;
    dcbSerialParams.StopBits = ONESTOPBIT;

    if (!SetCommState(hComm, &dcbSerialParams)) {
        fprintf(stderr, "Failed to set serial port state. Error: %ld\n", GetLastError());
        CloseHandle(hComm);
        return INVALID_HANDLE_VALUE;
    }

    COMMTIMEOUTS timeouts = { 0 };
    timeouts.ReadIntervalTimeout = 50;
    timeouts.ReadTotalTimeoutConstant = 50;
    timeouts.ReadTotalTimeoutMultiplier = 10;

    if (!SetCommTimeouts(hComm, &timeouts)) {
        fprintf(stderr, "Failed to set timeouts. Error: %ld\n", GetLastError());
        CloseHandle(hComm);
        return INVALID_HANDLE_VALUE;
    }

    return hComm;
}

double read_from_port_windows(HANDLE hComm) {
    char buffer[256] = { 0 };
    DWORD bytesRead = 0;

    if (!ReadFile(hComm, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
        fprintf(stderr, "Failed to read from serial port. Error: %ld\n", GetLastError());
        return -1.0;
    }

    if (bytesRead > 0) {
        buffer[bytesRead] = '\0';
        float result = strtof(buffer, NULL);
        return result;
    } else if (bytesRead == 0) {
        Sleep(100);
        return 0.0;
    }

    return -1.0;
}
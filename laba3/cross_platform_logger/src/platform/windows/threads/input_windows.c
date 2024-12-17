#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../../../../include/shared_memory/shared_memory.h"
#include "../../../../include/utils/mutex.h"
#include "../../../../include/threads/stop_execution.h"

extern int stop;

DWORD WINAPI input_windows(LPVOID arg) {
    HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if (hStdin == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Error: Unable to get standard input handle.\n");
        return 0;
    }

    DWORD originalMode;
    if (!GetConsoleMode(hStdin, &originalMode)) {
        fprintf(stderr, "Error: Unable to get console mode.\n");
        return 0;
    }

    SetConsoleMode(hStdin, originalMode & ~ENABLE_LINE_INPUT & ~ENABLE_ECHO_INPUT);

    while (1) {
        WaitForSingleObject(hStopMutex, INFINITE);
        if (stop) {
            ReleaseMutex(hStopMutex);
            SetConsoleMode(hStdin, originalMode);
            return 0;
        }
        ReleaseMutex(hStopMutex);

        WaitForSingleObject(hConsoleMutex, INFINITE);
        if (console_blocked) {
            ReleaseMutex(hConsoleMutex);
            Sleep(100);
            continue;
        }
        ReleaseMutex(hConsoleMutex);

        WaitForSingleObject(hConsoleMutex, INFINITE);
        DWORD events;
        if (GetNumberOfConsoleInputEvents(hStdin, &events) && events > 0) {
            INPUT_RECORD inputRecord;
            DWORD bytesRead;

            ReadConsoleInput(hStdin, &inputRecord, 1, &bytesRead);

            if (inputRecord.EventType == KEY_EVENT && inputRecord.Event.KeyEvent.bKeyDown) {
                char buffer[100];
                if (ReadConsole(hStdin, buffer, sizeof(buffer) - 1, &bytesRead, NULL)) {
                    buffer[bytesRead / sizeof(char)] = '\0';
                    buffer[strcspn(buffer, "\n")] = '\0';

                    int is_numeric = 1;
                    for (size_t i = 0; i < strlen(buffer); i++) {
                        if (!isdigit((unsigned char)buffer[i])) {
                            is_numeric = 0;
                            break;
                        }
                    }

                    if (!is_numeric) {
                        printf("Error: Input contains invalid characters. Please enter a valid number.\n");
                    } else {
                        int input = atoi(buffer);
                        printf("You entered: %d\n", input);

                        WaitForSingleObject(hCounterMutex, INFINITE);
                        *counter = input;
                        ReleaseMutex(hCounterMutex);
                    }
                } else {
                    printf("Error reading input\n");
                }
            }
        } else {
            printf("\rWaiting for input...\n");
            Sleep(1000);
        }
        ReleaseMutex(hConsoleMutex);
    }
    SetConsoleMode(hStdin, originalMode);

    return 0;
}

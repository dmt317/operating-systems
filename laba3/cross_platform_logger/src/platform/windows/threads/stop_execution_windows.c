#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../../../../include/threads/stop_execution.h"
#include "../../../../include/utils/mutex.h"

extern int stop;

int console_blocked = 0;

DWORD WINAPI stop_execution_windows(LPVOID arg) {
    while (1) {
        WaitForSingleObject(hStopMutex, INFINITE);
        if (stop) {
            ReleaseMutex(hStopMutex);
            return 0;
        }
        ReleaseMutex(hStopMutex);

        Sleep(10000);

        WaitForSingleObject(hConsoleMutex, INFINITE);
        console_blocked = 1;

        Sleep(100);
        printf("Do you want to continue? (y/n) ");
        fflush(stdout);

        DWORD start_time = GetTickCount();
        int timeout_ms = 9000;
        char answer = 0;
        int input_received = 0;

        while ((GetTickCount() - start_time) < (DWORD)timeout_ms) {
            DWORD events;
            INPUT_RECORD input_record;

            if (PeekConsoleInput(hConsoleInput, &input_record, 1, &events) && events > 0) {
                ReadConsoleInput(hConsoleInput, &input_record, 1, &events);

                if (input_record.EventType == KEY_EVENT && input_record.Event.KeyEvent.bKeyDown) {
                    answer = input_record.Event.KeyEvent.uChar.AsciiChar;

                    if (answer == 'y' || answer == 'n') {
                        input_received = 1;
                        break;
                    } else {
                        printf("Incorrect input! Try again.\n");
                    }
                }
            }

            Sleep(50);
        }

        WaitForSingleObject(hConsoleMutex, INFINITE);
        if (input_received) {
            if (answer == 'n') {
                WaitForSingleObject(hStopMutex, INFINITE);
                stop = 1;
                ReleaseMutex(hStopMutex);

                console_blocked = 0;
                ReleaseMutex(hConsoleMutex);

                return 0;
            }
        } else {
            printf("\nTimeout. Continuing...\n");
        }

        console_blocked = 0;
        ReleaseMutex(hConsoleMutex);
    }
    return 0;
}

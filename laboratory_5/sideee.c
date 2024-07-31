#include <windows.h>
#include <stdio.h>
#include <tchar.h>

#include "messages.h"

enum State
{
    waiting,
    ready,
} state = waiting;

BOOL isRunning = TRUE;
char fileBuff[1024] = { 0 };
int it = 0;

BOOL sendNextQuery(HWND to, HWND from) {
    COPYDATASTRUCT cds;
    cds.dwData = (int)INSERT;

    int read;
    char keyBuff[64] = { 0 };
    char valBuff[64] = { 0 };

    if (sscanf_s(fileBuff + it, "%63s %63s%n", keyBuff, 64, valBuff, 64, &read) != 2)
        return FALSE;
    it += read;
    if (fileBuff[it] == '\0')
        return FALSE;

    char combinedString[128];
    sprintf_s(combinedString, sizeof(combinedString), "%s %s", keyBuff, valBuff);
    cds.cbData = (DWORD)(strlen(combinedString) + 1);
    cds.lpData = combinedString;

    printf("%p -> %p: [%s] \"%s\"\n", (void*)from, (void*)to, tr(cds.dwData), (char*)cds.lpData);
    SendMessage((HWND)to, WM_COPYDATA, (WPARAM)from, (LPARAM)&cds);
    return TRUE;
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_COPYDATA:
        {
            PCOPYDATASTRUCT pCds = (PCOPYDATASTRUCT)lParam;
            COPYDATASTRUCT cds;
            char* receivedMsg = (char*)pCds->lpData;
            printf_s("%p <- %p: [%s] \"%s\"\n", (void*)wParam, (void*)hwnd, tr(pCds->dwData), receivedMsg);

            switch (pCds->dwData)
            {
                case START:
                    if (!sendNextQuery((HWND) wParam, hwnd)) {
                        isRunning = FALSE;
                    }
                    Sleep(1337 + rand()%100);
                    break;
                case OK:
                    if (!sendNextQuery((HWND) wParam, hwnd)) {
                        isRunning = FALSE;
                    }
                    break;
                case ERRCOL:
                    if (!sendNextQuery((HWND) wParam, hwnd)) {
                        isRunning = FALSE;
                    }

                    break;
                case END:
                    printf("Bye!\n");
                    isRunning = FALSE;
                    break;
                default:
                    //printf("Unexpected message: %s \n", receivedMsg);
                    break;
            }

        }
        case WM_CLOSE:
            DestroyWindow(hwnd);
            break;
        case WM_DESTROY:
            PostQuitMessage(0);
            break;
        default:
            return DefWindowProc(hwnd, message, wParam, lParam);
    }
    return 0;
}

int main() {
    const TCHAR CLASS_NAME[] = _T("Client Window Class");
    WNDCLASS wc = { 0 };
    HWND hwnd;
    MSG msg;

    FILE* fp;
    char lineBuff[256];

    fopen_s(&fp, "input.txt", "r");
    if (fp == NULL) {
        perror("Failed to open file");
        return 1;
    }

    int currentSize = 0;
    while (fgets(lineBuff, sizeof(lineBuff), fp) != NULL) {
        int lineLength = strlen(lineBuff);

        if (currentSize + lineLength < 1024) {
            strcat_s(fileBuff, sizeof fileBuff, lineBuff);
            currentSize += lineLength;
        }
    }
    fclose(fp);

    Sleep(1000);

    wc.lpfnWndProc = WndProc;
    wc.hInstance = GetModuleHandle(NULL);
    wc.lpszClassName = CLASS_NAME;

    if (!RegisterClass(&wc)) {
        printf("Window registration failed!\n");
        return 1;
    }

    hwnd = CreateWindowEx(
            0,                              // Optional window styles.
            CLASS_NAME,                     // Window class
            _T("Client Window"),            // Window text
            WS_OVERLAPPEDWINDOW,            // Window style

            // Size and position
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

            NULL,       // Parent window
            NULL,       // Menu
            wc.hInstance, // Instance handle
            NULL        // Additional application data
    );

    printf("Client HWND: %p\n", (void*)hwnd);

    if (hwnd == NULL) {
        printf("Window creation failed!\n");
        return 1;
    }

    //srand((unsigned int) hwnd);

    ShowWindow(hwnd, SW_HIDE);

    while (isRunning) {
        while (GetMessage(&msg, NULL, 0, 0)) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return 0;
}

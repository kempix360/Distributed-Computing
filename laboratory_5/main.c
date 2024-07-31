#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <tchar.h>

#include "tree.h"
#include "messages.h"

#define EXE_PATH "C:\\Users\\ArthurMorgan\\OneDrive\\Desktop\\4_semestr\\Przetwarzanie_rozproszone\\laboratory_5\\side.exe"
#define CWD "C:\\Users\\ArthurMorgan\\OneDrive\\Desktop\\4_semestr\\Przetwarzanie_rozproszone\\laboratory_5"
#define COUNT 3

typedef struct {
    DWORD targetProcessID;
    HWND foundHwnd;
    TCHAR targetClassName[256];
} EnumWindowsData;

RedBlackTree* tree;

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);
HWND FindClientWindow(HANDLE hProcess);
BOOL spawnClients(HWND hArray[], PROCESS_INFORMATION piArray[], STARTUPINFO siArray[]);

BOOL handleInsert(char* data) {
    char key[64];
    char value[64];

    if (sscanf_s(data, "%63s %63s", key, sizeof(key), value, sizeof(value)) != 2)
        return FALSE; // Error of parsing

    if (checkKeyExistence(key, tree))
        return FALSE; // Key already exists

    insertValue(key, value, tree);
    return TRUE; // Success
}

void sendMsg(HWND to, HWND from, MsgType type, const char* data, int size) {
    COPYDATASTRUCT cds;
    cds.dwData = (int)type;
    cds.cbData = size;
    cds.lpData = (PVOID)data;
    printf_s("Main: %p -> Side: %p: [%s] \"%s\"\n", (void*)from, (void*)to, tr(cds.dwData), data);
    SendMessage((HWND)to, WM_COPYDATA, (WPARAM)from, (LPARAM)&cds);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
        case WM_COPYDATA:
        {
            PCOPYDATASTRUCT pCds = (PCOPYDATASTRUCT)lParam;
            char* receivedMsg = (char*)pCds->lpData;
            printf_s("Main: %p <- Side: %p: [%s] \"%s\"\n", (void*)wParam, (void*)hwnd, tr(pCds->dwData), receivedMsg);
            switch (pCds->dwData)
            {
                case START:
                    Sleep(500);
                case INSERT:
                    Sleep(500);
                    if (handleInsert(receivedMsg)) {
                        sendMsg((HWND)wParam, hwnd, OK, 0, 0);
                    }
                    else
                    {
                        const char* data = "collision with an existing record";
                        int size = strlen(data) + 1;
                        sendMsg((HWND)wParam, hwnd, ERRCOL, data, size);
                    }
                case SEARCH:
                    // search
                default:
                    //printf("Unexpected message: %s \n", receivedMsg);
                    break;
            }
            return 0;
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

int main(int argc, char* argv[]) {
    const TCHAR CLASS_NAME[] = _T("Server Window Class");
    WNDCLASS wc = { 0 };
    HWND hwnd;
    MSG msg;
    BOOL running = TRUE;

    tree = (RedBlackTree*)malloc(sizeof(RedBlackTree));
    tree->root = NULL;

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
            _T("Server Window"),            // Window text
            WS_OVERLAPPEDWINDOW,            // Window style

            // Size and position
            CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

            NULL,       // Parent window
            NULL,       // Menu
            wc.hInstance, // Instance handle
            NULL        // Additional application data
    );

    printf("Server HWND: %p\n", (void*)hwnd);

    if (hwnd == NULL) {
        printf("Window creation failed!\n");
        return 1;
    }

    ShowWindow(hwnd, SW_HIDE);

    HWND hArray[COUNT];
    PROCESS_INFORMATION piArray[COUNT];
    STARTUPINFO siArray[COUNT];

    if (!spawnClients(hArray, piArray, siArray)) {
        printf("spawnClients failed. Exiting...\n");
        return 0;
    }

    for (int i = 0; i < COUNT; i++)
    {
        sendMsg(hArray[i], hwnd, START, 0, 0);
    }

    Sleep(500);

    while (1)
    {
        GetMessage(&msg, NULL, 0, 0);
        TranslateMessage(&msg);
        DispatchMessage(&msg);
        print(tree);
    }

    for (int i = 0; i < COUNT; i++)
    {
        CloseHandle(piArray[i].hProcess);
        CloseHandle(piArray[i].hThread);
    }
    return 0;
}

BOOL spawnClients(HWND hArray[], PROCESS_INFORMATION piArray[], STARTUPINFO siArray[]) {
    for (int i = 0; i < COUNT; i++) {
        ZeroMemory(&piArray[i], sizeof(PROCESS_INFORMATION));
        ZeroMemory(&siArray[i], sizeof(STARTUPINFO));
    }

    for (int i = 0; i < COUNT; i++)
    {
        siArray[i].cb = sizeof(siArray[i]);
        siArray[i].dwFlags = STARTF_USESHOWWINDOW;
        siArray[i].wShowWindow = SW_SHOW;

        TCHAR cmdLine[] = TEXT(EXE_PATH);
        TCHAR cwd[] = TEXT(CWD);
        if (!CreateProcess(
                NULL,               // No module name (use command line)
                cmdLine,            // Command line to execute
                NULL,               // Process handle not inheritable
                NULL,               // Thread handle not inheritable
                FALSE,              // Set handle inheritance to FALSE
                CREATE_NEW_CONSOLE, // No creation flags
                NULL,               // Use parent's environment block
                cwd,                // Use parent's starting directory or specify NULL to use the current directory
                &siArray[i],        // Pointer to STARTUPINFO structure
                &piArray[i]         // Pointer to PROCESS_INFORMATION structure
        )) {
            printf("CreateProcess failed (%lu).\n", GetLastError());
            return FALSE;
        }
    }

    Sleep(3000);

    for (int i = 0; i < COUNT; i++)
    {
        HWND window = FindClientWindow(piArray[i].hProcess);
        hArray[i] = window;
    }
    return TRUE;
}

BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam) {
    EnumWindowsData* data = (EnumWindowsData*)lParam;
    DWORD processID;
    TCHAR className[256];

    GetWindowThreadProcessId(hwnd, &processID);
    GetClassName(hwnd, className, 256);


    if (processID == data->targetProcessID && _tcscmp(className, data->targetClassName) == 0) {
        printf("Found window handle: %p\n", (void*)hwnd);
        data->foundHwnd = hwnd;
        return FALSE;
    }

    return TRUE;
}

HWND FindClientWindow(HANDLE hProcess) {
    DWORD processID = GetProcessId(hProcess);
    if (processID == 0) {
        printf("GetProcessId failed (%lu).\n", GetLastError());
        return NULL;
    }

    printf("Searching for windows of process ID: %lu\n", processID);

    EnumWindowsData data;
    data.targetProcessID = processID;
    data.foundHwnd = NULL;
    _tcscpy_s(data.targetClassName, _countof(data.targetClassName), TEXT("Client Window Class"));

    EnumWindows(EnumWindowsProc, (LPARAM)&data);

    if (data.foundHwnd != NULL) {
        printf("Window found: %p\n", (void*)data.foundHwnd);
    }
    else {
        printf("No window found for process ID: %lu\n", processID);
    }
    return data.foundHwnd;
}
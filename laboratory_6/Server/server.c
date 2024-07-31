#define _CRT_SECURE_NO_WARNINGS
#define MAX_STRING_SIZE 80
#define TIMEOUT_SECONDS 10 // Okres czasu (w sekundach) bezczynno�ci, po kt�rym program zako�czy dzia�anie

#include <stdio.h>
#include <winsock2.h>
#include <stddef.h>
#include <stdlib.h>
#include <time.h>
#include "tree.h"
#include "windows.h"


// Funkcja tworz�ca serwerowe gniazdko
SOCKET createServerSocket() {
    WSADATA wsas;
    int result;
    WORD wersja;
    wersja = MAKEWORD(2, 0);

    // WSAStartup
    result = WSAStartup(wersja, &wsas);
    if (result != 0) {
        printf("WSAStartup error");
        return 1;
    }

    // Tworzenie gniazdka
    SOCKET serverSocket;
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        printf("Socket creating error");
        return 2;
    }

    // Struktura adresowa
    struct sockaddr_in serverAdressStruct;
    memset((void*)(&serverAdressStruct), 0, sizeof(serverAdressStruct));
    serverAdressStruct.sin_family = AF_INET;
    serverAdressStruct.sin_port = htons(1234);
    serverAdressStruct.sin_addr.s_addr = htonl(INADDR_ANY);

    // Binding
    result = bind(serverSocket, (struct sockaddr FAR*) & serverAdressStruct, sizeof(serverAdressStruct));
    if (result == SOCKET_ERROR) {
        printf("Binding error");
        return 3;
    }

    // Start listening
    result = listen(serverSocket, 5);
    if (result != 0) {
        printf("Listen error");
        return 4;
    }

    return serverSocket;
}

// Funkcja do oczekiwania na po��czenia z zadanym timeoutem
int waitForConnections(SOCKET serverSocket, int timeout_seconds) {
    struct timeval timeout;
    timeout.tv_sec = timeout_seconds;
    timeout.tv_usec = 0;

    fd_set fds;
    FD_ZERO(&fds);
    FD_SET(serverSocket, &fds);

    int activity = select(0, &fds, NULL, NULL, &timeout);
    if (activity == SOCKET_ERROR) {
        printf("Select error");
        return 1;
    }
    else if (activity == 0) {
        printf("Timeout reached. No connections received.\n\n");
        return 2;
    }

    return 0;
}

int main() {
    SOCKET serverSocket = createServerSocket();

    // Waiting for connections
    SOCKET clientSocket;
    struct sockaddr_in clientAdressStruct;
    int lenc = sizeof(clientAdressStruct);

    char buffer[MAX_STRING_SIZE];
    char key[MAX_STRING_SIZE];
    char value[MAX_STRING_SIZE];
    RedBlackTree tree;
    tree.root = NULL;
    srand(time(NULL));

    printf("SERVER\n");

    while (1) {
        int connectionStatus = waitForConnections(serverSocket, TIMEOUT_SECONDS);
        if (connectionStatus == 1 || connectionStatus == 2) // 1 - b��d select, 2 - timeout
            break;

        clientSocket = accept(serverSocket, (struct sockaddr FAR*) & clientAdressStruct, &lenc);
        while (1) {
            if (recv(clientSocket, buffer, sizeof(buffer), 0) > 0) {
                sscanf(buffer, "%s %s", key, value);
                printf("%s\n", buffer);
                if (doesKeyAlreadyExist(key, &tree) == 0) {
                    insertValue(key, value, &tree);
                    strcpy(buffer, "OK");
                }
                else {
                    strcpy(buffer, "NOT OK");
                }

                int length = strlen(buffer);
                buffer[length] = '\0';
                int bytesSent;
                do {
                    bytesSent = send(clientSocket, buffer, length + 1, 0);
                } while (bytesSent == SOCKET_ERROR);
                break;
            }
        }
        closesocket(clientSocket);
    }

    // Koniec programu
    closesocket(serverSocket);
    WSACleanup();

    printf("ZAWARTOSC DRZEWA\n");
    print(&tree);

    system("pause");
    return 0;
}

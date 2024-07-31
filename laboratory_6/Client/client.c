#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define NUMBER_OF_CLIENTS 3
#define MAX_STRING_SIZE 80
#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <winsock2.h> // Winsock2.h zawiera deklaracje funkcji API Winsock
#include <stdlib.h>
#include <time.h>
#include "windows.h"

SOCKET connectToServer(char* IP) {
    SOCKET clientSocket;
    struct sockaddr_in clientAdressStruct;
    WSADATA wsas;
    WORD wersja;
    wersja = MAKEWORD(2, 0);

    int result;
    result = WSAStartup(wersja, &wsas);
    if (result != 0) {
        printf("WSAStartup error");
        return 1;
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == -1) {
        printf("Socket creating error");
        return 2;
    }
    memset((void*)(&clientAdressStruct), 0, sizeof(clientAdressStruct));
    clientAdressStruct.sin_family = AF_INET;
    clientAdressStruct.sin_port = htons(1234);
    clientAdressStruct.sin_addr.s_addr = inet_addr(IP);

    result = connect(clientSocket, (struct sockaddr FAR*) &clientAdressStruct, sizeof(clientAdressStruct));
    if (result == SOCKET_ERROR)
    {
        printf("Blad polaczenia!\n");
        return 3;
    }
    return clientSocket;
}

int main(int argc, char** argv) {
    int length;
    char buffer[MAX_STRING_SIZE];
    char* filename = argv[1];
    char key[MAX_STRING_SIZE];
    char value[MAX_STRING_SIZE];
    srand(time(NULL));


    printf("CLIENT\n");
    Sleep(2000);

    FILE* file = fopen(filename, "r");
    if (file == NULL) {
        printf("Cannot open file: %s\n", filename);
        return 1;
    }


    while (fscanf(file, "%s %s", key, value) == 2) {
        sprintf(buffer, "%s %s", key, value);
        printf("%s\n", buffer);
        length = strlen(buffer);
        buffer[length] = '\0';

        SOCKET clientSocket = connectToServer(argv[2]);

        int bytesSent;
        //sending data
        do {
            bytesSent = send(clientSocket, buffer, length + 1, 0);
        } while (bytesSent == SOCKET_ERROR);

        //waiting for response
        while(1){
            if (recv(clientSocket, buffer, sizeof(buffer), 0) > 0) {
                printf("%s\n\n", buffer);
                break;
            }
        }
        closesocket(clientSocket);
        Sleep(rand()%3500 + 500);
    }
    fclose(file);
    printf("Zakonczono przetwarzanie\n");


    WSACleanup();
    system("pause");
    return 0;
}
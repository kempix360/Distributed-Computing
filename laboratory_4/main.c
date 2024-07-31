#include <stdio.h>
#include <stdlib.h>
#include <direct.h>
#include "tree.h"
#include <Windows.h>

typedef struct Args {
    char** argv;
    int argc;
    char* buffer[2];
    HANDLE semaphore;
    HANDLE mainThreadEvent;
    int finished;
} Args;

DWORD WINAPI Thread (void* param) {
    Args* arguments = (Args *) param;
    char** keys = (char **)malloc(arguments->argc * sizeof(char*));
    char** values = (char **)malloc(arguments->argc * sizeof(char*));
    for (int i = 0; i < arguments->argc/2; i++) {
        keys[i] = arguments->argv[i];
        values[i] = arguments->argv[i+1];
        arguments->buffer[0] = keys[i];
        arguments->buffer[1] = values[i];
        SetEvent(arguments->semaphore);
        WaitForSingleObject(arguments->mainThreadEvent, INFINITE);
        ResetEvent(arguments->mainThreadEvent);
    }
    arguments->finished = 1;
    return 0;
}

int main() {
    HANDLE threads[2];

    FILE *file;
    char line[100];
    char* key;
    char* value;

    Args args_array[2];
    for (int i = 0; i < 2; i++) {
        args_array[i].argc = 0;
        args_array[i].argv = (char**)malloc(12 * sizeof(char*));
        args_array[i].semaphore = CreateEventA(NULL, TRUE, FALSE, NULL);
        args_array[i].mainThreadEvent = CreateEventA(NULL, TRUE, FALSE, NULL);
        args_array[i].finished = 0; // Inicjalizacja zmiennej zakończenia pracy wątku
    }

    file = fopen("file.in", "r");
    if (!file) {
        perror("Error opening file");
        return 1;
    }

    for (int i = 0 ;fgets(line, sizeof(line), file) != NULL; i=i+2) {
        key = (char*)malloc((strlen(line) + 1) * sizeof(char));
        value = (char*)malloc((strlen(line) + 1) * sizeof(char));
        sscanf(line, "%s", key);
        fgets(line, sizeof(line), file);
        sscanf(line, "%s", value);
        args_array[0].argv[i] = strdup(key);
        args_array[1].argv[i] = strdup(key);
        args_array[0].argv[i+1] = strdup(value);
        args_array[1].argv[i+1] = strdup(value);
        args_array[0].argc +=2;
        args_array[1].argc +=2;
        printf("Key: %s, value: %s\n", key, value);
    }
    fclose(file);

    threads[0] = CreateThread(NULL, 0, Thread, &args_array[0], 0, NULL);
    threads[1] = CreateThread(NULL, 0, Thread, &args_array[1], 0, NULL);

    RedBlackTree* tree = (RedBlackTree*)malloc(sizeof(RedBlackTree));
    tree->root = NULL;
    char** list_of_keys = (char **)malloc(100 * sizeof(char*));
    int size_of_list = 0;

    while (1) {
        int all_threads_finished = 1;
        for (int i = 0; i < 2; i++) {
            WaitForSingleObject(args_array[i].semaphore, INFINITE);
            char* k;
            char* v;
            k = args_array[i].buffer[0];
            v = args_array[i].buffer[1];
            int exists = 0;

            for (int j = 0; j < size_of_list; j++){
                if (strcmp(list_of_keys[j], k) == 0){
                    printf("Thread[%d]: Key already exists\n", i);
                    exists = 1;
                    break;
                }
            }
            if (!exists && k != NULL && v!=NULL) {
                list_of_keys[size_of_list] = k;
                size_of_list++;
                insertValue(k, v, tree);
                printf("Thread[%d]: Successfully inserted value: %s\n", i, v);
                print(tree);
            }
            SetEvent(args_array[i].mainThreadEvent);
            all_threads_finished &= args_array[i].finished;
        }
        if (all_threads_finished)
            break;
    }

    WaitForMultipleObjects(2, threads, TRUE, INFINITE);

    for (int i = 0; i < 2; i++) {
        CloseHandle(args_array[i].semaphore);
        CloseHandle(args_array[i].mainThreadEvent);
        free(args_array[i].argv);
    }

    free(tree);
    free(list_of_keys);
    return 0;
}

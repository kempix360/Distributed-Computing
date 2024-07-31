#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include "tree.h"

#define BUFFER_SIZE 256
#define THREAD_COUNT 3
#define RUN_DURATION 20 // Czas działania wątków w sekundach

typedef struct {
    char buffer[BUFFER_SIZE];
    int buffer_ready;
    char response[BUFFER_SIZE];
    long thread_id;
} ThreadData;

volatile int stop_threads = 0; // Flaga sygnalizująca zatrzymanie wątków
pthread_mutex_t buffer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t tree_mutex = PTHREAD_MUTEX_INITIALIZER;

void* thread_function(void* arg) {
    ThreadData* data = (ThreadData*)arg;
    char filename[20];
    snprintf(filename, sizeof(filename), "file%ld.txt", data->thread_id);

    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    char key[BUFFER_SIZE];
    char value[BUFFER_SIZE];

    while (1) {
        pthread_mutex_lock(&buffer_mutex);
        if (stop_threads) {
            pthread_mutex_unlock(&buffer_mutex);
            break;
        }
        pthread_mutex_unlock(&buffer_mutex);

        // Read key-value pairs from the file
        if (fscanf(file, "%s %s", key, value) == EOF) {
            rewind(file); // Reset file pointer to the beginning
            continue;     // Restart from the beginning of the file
        }

        // Copy key-value pair to the buffer
        pthread_mutex_lock(&buffer_mutex);
        snprintf(data->buffer, BUFFER_SIZE, "%ld %s %s", data->thread_id, key, value);
        data->buffer_ready = 1;
        pthread_mutex_unlock(&buffer_mutex);

        // Wait for server response
        while (1) {
            pthread_mutex_lock(&buffer_mutex);
            if (!data->buffer_ready || stop_threads) {
                pthread_mutex_unlock(&buffer_mutex);
                break;
            }
            pthread_mutex_unlock(&buffer_mutex);
            usleep(100); // Busy wait for response
        }
        if (stop_threads) break;

        printf("THREAD %ld: Server response: %s\n\n", data->thread_id, data->response);

        sleep(2); // Simulate work
    }

    fclose(file);
    return NULL;
}

int main() {
    pthread_t threads[THREAD_COUNT];
    ThreadData thread_data[THREAD_COUNT];

    // Initialize thread data and create threads
    for (long i = 0; i < THREAD_COUNT; i++) {
        thread_data[i].thread_id = i + 1;
        thread_data[i].buffer_ready = 0;
        pthread_create(&threads[i], NULL, thread_function, &thread_data[i]);
    }

    RedBlackTree tree;
    tree.root = NULL;

    // Start timer
    time_t start_time = time(NULL);

    while (1) {
        // Check if run duration has elapsed
        pthread_mutex_lock(&buffer_mutex);
        if (difftime(time(NULL), start_time) >= RUN_DURATION) {
            stop_threads = 1;
            pthread_mutex_unlock(&buffer_mutex);
            break;
        }
        pthread_mutex_unlock(&buffer_mutex);

        for (int i = 0; i < THREAD_COUNT; i++) {
            pthread_mutex_lock(&buffer_mutex);
            if (thread_data[i].buffer_ready) {
                // Extract key and value from the buffer and insert into data structure
                char key[BUFFER_SIZE];
                char value[BUFFER_SIZE];
                long tid;
                sscanf(thread_data[i].buffer, "%ld %s %s", &tid, key, value);
                printf("THREAD %ld: Main program received: Key: %s, Value: %s\n", tid, key, value);

                pthread_mutex_lock(&tree_mutex);
                if (doesKeyAlreadyExist(key, &tree) == 0) {
                    char* key_copy = strdup(key);
                    char* value_copy = strdup(value);
                    insertValue(key_copy, value_copy, &tree);
                    printf("THREAD %ld:     OK inserted: Key: %s, Value: %s\n\n", tid, key, value);
                    snprintf(thread_data[i].response, BUFFER_SIZE, "OK");
                } else {
                    printf("THREAD %ld:     NOT OK Key: %s already exists in the tree.\n\n", tid, key);
                    snprintf(thread_data[i].response, BUFFER_SIZE, "NOT OK");
                }
                pthread_mutex_unlock(&tree_mutex);

                thread_data[i].buffer_ready = 0; // Signal that the buffer has been processed
            }
            pthread_mutex_unlock(&buffer_mutex);
        }

        usleep(100); // Prevent busy waiting from consuming too much CPU
    }

    // Join threads
    for (int i = 0; i < THREAD_COUNT; i++) {
        pthread_join(threads[i], NULL);
    }

    // Print tree contents
    printf("ZAWARTOSC DRZEWA\n");
    pthread_mutex_lock(&tree_mutex);
    print(&tree);
    pthread_mutex_unlock(&tree_mutex);

    return 0;
}

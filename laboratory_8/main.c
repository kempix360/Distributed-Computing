#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

// Struktura dla przechowywania par klucz-wartość
typedef struct {
    int key;
    int value;
} KeyValuePair;

// Funkcja do odczytu par klucz-wartość z pliku
void read_pairs_from_file(const char* filename, int pipe_fd) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    KeyValuePair kvp;
    while (fscanf(file, "%d %d", &kvp.key, &kvp.value) != EOF) {
        write(pipe_fd, &kvp, sizeof(KeyValuePair));
    }

    fclose(file);
    close(pipe_fd);
}

int main() {
    int pipefd1[2], pipefd2[2];
    pid_t cpid1, cpid2;

    if (pipe(pipefd1) == -1 || pipe(pipefd2) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    cpid1 = fork();
    if (cpid1 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid1 == 0) { // Proces potomny 1
        close(pipefd1[0]); // Zamknięcie końca do odczytu
        read_pairs_from_file("file1.txt", pipefd1[1]);
        exit(EXIT_SUCCESS);
    }

    cpid2 = fork();
    if (cpid2 == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (cpid2 == 0) { // Proces potomny 2
        close(pipefd2[0]); // Zamknięcie końca do odczytu
        read_pairs_from_file("file2.txt", pipefd2[1]);
        exit(EXIT_SUCCESS);
    }

    // Proces główny
    close(pipefd1[1]);
    close(pipefd2[1]);

    KeyValuePair hash_map[100];
    memset(hash_map, 0, sizeof(hash_map));

    KeyValuePair kvp;
    while (read(pipefd1[0], &kvp, sizeof(KeyValuePair)) > 0) {
        insert_to_hash_map(hash_map, kvp, 100);
    }

    while (read(pipefd2[0], &kvp, sizeof(KeyValuePair)) > 0) {
        insert_to_hash_map(hash_map, kvp, 100);
    }

    close(pipefd1[0]);
    close(pipefd2[0]);

    wait(NULL); // Czekanie na zakończenie procesów potomnych
    wait(NULL);

    // Wypisanie zawartości hash mapy
    for (int i = 0; i < 100; ++i) {
        if (hash_map[i].key != 0 || hash_map[i].value != 0) {
            printf("Key: %d, Value: %d\n", hash_map[i].key, hash_map[i].value);
        }
    }

    return 0;
}

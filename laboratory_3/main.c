#include <stdio.h>
#include <stdlib.h>
#include "tree.h"
#include <unistd.h>

int main() {
    int n; // number of keys
    printf("Enter number of arguments:\n");
    scanf("%d", &n);
    char** argv = (char **)malloc((2*n + 2) * sizeof(char*));
    argv[0] = "process_2.out";
    for (int i = 1; i < 2*n + 1; i++) {
        if (i % 2 != 0)
            printf("Enter key and value:\n");
        argv[i] = (char *)malloc(100 * sizeof(char));
        scanf("%s", argv[i]);
    }
    argv[2*n + 1] = NULL;
    for (int i = 1; i < 2*n + 1; i=i+2) {
        printf("Key: %s, value: %s\n", argv[i], argv[i + 1]);
    }

    print(tree);
    execv("process_2.out", argv);

    return 0;
}
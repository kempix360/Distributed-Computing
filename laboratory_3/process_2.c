#include <stdio.h>
#include <stdlib.h>
#include "tree.h"

int main(int argc, char* argv[]) {
    char** keys = (char **)malloc(argc * sizeof(char*));
    char** values = (char **)malloc(argc * sizeof(char*));
    int counter = 0;
    for (int i = 1; i < argc; i++) {
        if (i % 2 != 0)
            keys[counter] = argv[i];
        else {
            values[counter] = argv[i];
            counter++;
        }
    }

    RedBlackTree* tree = (RedBlackTree*)malloc(sizeof(RedBlackTree));
    tree->root = NULL;
    int i = 0;
    while (i < counter){
        insertValue(keys[i], values[i], tree);
        i++;
    }

    print(tree);

    return 0;
}
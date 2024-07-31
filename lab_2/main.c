#include <stdio.h>
#include <stdlib.h>
#include "tree.h"

int main() {
    RedBlackTree* tree = (RedBlackTree*)malloc(sizeof(RedBlackTree));
    tree->root = NULL;

// Example usage
    insertValue("klucz_1", "12587", tree);
    insertValue("klucz_2", "hello", tree);
    insertValue("key_1", "yeeeee", tree);
    insertValue("key_2", "abab", tree);
    insertValue("klucz_3", "baba", tree);
    insertValue("klucz_4", "rrrrr", tree);
    insertValue("key_3", "number 1", tree);
    insertValue("key_4", "value_18", tree);
    insertValue("klucz_5", "12587", tree);
    insertValue("klucz_6", "hello", tree);
    insertValue("key_5", "yeeeee", tree);
    insertValue("key_6", "abab", tree);
    insertValue("klucz_7", "baba", tree);
    insertValue("klucz_8", "rrrrr", tree);
    insertValue("key_7", "number 1", tree);
    insertValue("kez_6", "value_18", tree);

    print(tree);

    getValue("kez_6", tree);

    clear_memory(tree->root);
    free(tree);
    return 0;
}
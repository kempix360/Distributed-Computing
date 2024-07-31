#include <stdio.h>
#include "header.h"

int compareHash(void* hash1, void* hash2) {
    const int* value1 = hash1;
    const int* value2 = hash2;
    if (*value1 > *value2)
        return 1;
    else if (*value1 == *value2)
        return 0;
    else return -1;
}
void* hashing_func(void* value) {
    char* hash = value;
    int sum = 0;

    for (int i = 0; i < strlen(hash); i++) {
        sum += (int)hash[i];
    }

    int* result = (int*)malloc(sizeof(int));
    if (result == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }

    *result = sum;
    return result;
}

void print_value(void* value){

}

int main() {
    ListManager* manager = malloc(sizeof(ListManager));
    manager->hashList = NULL;
    manager->f_hash = hashing_func;
    manager->f_comp = compareHash;
    manager->f_print = print_value;

// Example usage
    insert("12587", manager);
    insert("hello", manager);
    insert("yeeeee", manager);
    insert("abab", manager);
    insert("baba", manager);
    insert("rrrrr", manager);
    insert("number 1", manager);
    insert("value_18", manager);

    print(manager);

    int y = 741;
    int x = 391;
    char* string = "hello";
    void* value = get_value(manager->f_hash(string), manager);
    if (value != NULL)
        printf("\nGet value: %s\n", (char*)value);

    delete_value(&x,manager); //invalid

    x = 390;
    delete_value(&x,manager);
    delete_value(&y,manager);
    print(manager);
    delete_value(&x,manager);
    print(manager);

    value = get_value(&x, manager);
    if (value != NULL)
        printf("\nGet value: %s\n", (char*)value);

    clean_up(manager);
    free(manager);
    return 0;
}
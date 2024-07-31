#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Define a node structure for a singly linked list
typedef struct ListNode {
    void* data;               // Pointer to data
    struct ListNode* next;   // Pointer to the next node
} ListNode;

// Define a node structure for a doubly linked list
typedef struct HashNode {
    void* hash;              // Hash value
    ListNode* list;         // Pointer to the head of the linked list containing values for this hash
    struct HashNode* next;  // Pointer to the next node in the doubly linked list
    struct HashNode* prev; // Pointer to the previous node in the doubly linked list
} HashNode;

// Define the structure for the list manager
typedef struct ListManager {
    void* (*f_hash)(void*);
    void (*f_print)(void*);// Function pointer for insertion
    int (*f_comp)(void*, void*);                           // Function pointer for comparison     // Function pointer for deletion
    HashNode* hashList;                                 // Pointer to the head of the doubly linked list
} ListManager;

void print(ListManager* manager) {
    HashNode* currentHash = manager->hashList;
    int index = 0;
    while (currentHash != NULL) {
        printf("Hash[%d]: %d\n", index, *((int*)currentHash->hash));
        index++;
        printf("Values: ");

        ListNode* currentList = currentHash->list;
        while (currentList != NULL) {
            printf("%s, ", (char*)currentList->data);
            currentList = currentList->next;
        }
        printf("\n");
        currentHash = currentHash->next;
    }
    printf("--------------------------------------\n");
}

// Function to create a new ListNode
ListNode* createListNode(void* data) {
    ListNode* newNode = (ListNode*)malloc(sizeof(ListNode));
    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    newNode->data = data;
    newNode->next = NULL;
    return newNode;
}

// Function to create a new HashNode
HashNode* createHashNode(void* hash) {
    HashNode* newNode = (HashNode*)malloc(sizeof(HashNode));
    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    newNode->hash = hash; // Duplicate the hash string
    newNode->list = NULL;
    newNode->next = NULL;
    newNode->prev = NULL;
    return newNode;
}

// Function to insert a value into the list
void insert(void* data, ListManager* manager) {
    void* hash = manager->f_hash(data);

    // Find the appropriate position to insert the new hashnode
    HashNode* currentHash = manager->hashList;
    HashNode* prevHash = NULL;

    while (currentHash != NULL && manager->f_comp(currentHash->hash, hash) < 0) {
        prevHash = currentHash;
        currentHash = currentHash->next;
    }

    // If already exists
    if (currentHash != NULL && manager->f_comp(currentHash->hash, hash) == 0) {
        free(hash);
        ListNode* newNode = createListNode(data);
        newNode->next = currentHash->list;
        currentHash->list = newNode;
        return;
    }
    else { // If the hash doesn't exist, create a new HashNode
        HashNode* newNode = createHashNode(hash);
        newNode->list = createListNode(data);

        if (prevHash == NULL) {
            newNode->next = manager->hashList;
            if (manager->hashList != NULL)
                manager->hashList->prev = newNode;
            manager->hashList = newNode;
        } else {
            newNode->next = currentHash;
            newNode->prev = prevHash;
            if (prevHash->next != NULL)
                currentHash->prev = newNode;
            prevHash->next = newNode;
        }
    }
}

void* get_value(void* hash, ListManager* manager){
    HashNode* currentHash = manager->hashList;

    while (currentHash != NULL && manager->f_comp(currentHash->hash, hash) != 0) {
        currentHash = currentHash->next;
    }
    if (currentHash == NULL){
        printf("\nHash is invalid!!!!!!!\n");
        return NULL;
    }
    ListNode* firstNode = currentHash->list;
    return firstNode->data;
}

void delete_value(void* hash, ListManager* manager) {
    HashNode* currentHash = manager->hashList;

    // Search for the hash in the doubly linked list
    while (currentHash != NULL && manager->f_comp(currentHash->hash, hash) != 0) {
        currentHash = currentHash->next;
    }

    if (currentHash == NULL) {
        printf("\nHash is invalid!!!!!!!\n");
        return; // Hash not found
    }

    // Find the last node in the linked list
    ListNode* prev = NULL;
    ListNode* current = currentHash->list;
    while (current != NULL && current->next != NULL) {
        prev = current;
        current = current->next;
    }

    // Remove the last node from the list
    if (prev == NULL)
        currentHash->list = NULL; // If only one node in the list
    else
        prev->next = NULL;
    printf("\nDeleted value successfully\n");
    free(current);

    // If the list is empty, delete the hash node
    if (currentHash->list == NULL) {
        if (currentHash->prev == NULL)
            manager->hashList = currentHash->next;
        else
            currentHash->prev->next = currentHash->next;

        if (currentHash->next != NULL)
            currentHash->next->prev = currentHash->prev;

        free(currentHash->hash);
        free(currentHash);
    }
}

void clean_up(ListManager* manager) {
    HashNode* currentHash = manager->hashList;

    while (currentHash != NULL) {
        ListNode* currentList = currentHash->list;
        while (currentList != NULL) {
            ListNode* nextList = currentList->next;
            free(currentList);
            currentList = nextList;
        }
        HashNode* nextHash = currentHash->next;
        free(currentHash->hash);
        free(currentHash);
        currentHash = nextHash;
    }
    manager->hashList = NULL;
}
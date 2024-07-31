#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct RedBlackTree {
    struct TreeNode* root;
} RedBlackTree;

// Define a node structure for a singly linked list
typedef struct TreeNode {
    char* value;
    char* key;
    int hash;
    int color;  //red - 1 black - 0
    struct TreeNode* p;
    struct TreeNode* r;
    struct TreeNode* l;
} TreeNode;

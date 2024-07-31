#ifndef TREE_H_
#define TREE_H_

// Define a node structure for a singly linked list
typedef struct TreeNode {
    char* value;
    char* key;
    int hash;
    int color;  // red - 1 black - 0
    struct TreeNode* p;
    struct TreeNode* r;
    struct TreeNode* l;
} TreeNode;

typedef struct RedBlackTree {
    struct TreeNode* root;
} RedBlackTree;

extern TreeNode* root;

int compareHash(int hash1, int hash2);
int hashing_func(char* hash);
TreeNode* createTreeNode(char* key, char* value);
void rotateLeft(TreeNode* node);
void rotateRight(TreeNode* node);
void fixDoubleRed(TreeNode* x);
void insertValue(char* key, char* value);
void printNode(TreeNode* node, int level);
void print();
TreeNode* searchForKey(TreeNode* node, char* key);
int doesKeyAlreadyExist(char* key);
void getValue(char* key);
void deleteValue();
void clear_memory();

#endif /* TREE_H_ */

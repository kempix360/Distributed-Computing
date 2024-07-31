#include "tree.h"

void printNode(TreeNode* root, int level) {
    if (root == NULL) {
        return;
    }
    printf("(value: %s, key: %s, hash: %d, color: %s)\n", root->value, root->key, root->hash, root->color == 1 ? "red" : "black");

    if (root->l != NULL) {
        for (int i = 0; i < level; i++) {
            printf("  ");
        }
        printf("Left: ");
        printNode(root->l, level + 1);
    }

    if (root->r != NULL) {
        for (int i = 0; i < level; i++) {
            printf("  ");
        }
        printf("Right: ");
        printNode(root->r, level + 1);
    }
}

void print(RedBlackTree* tree) {
    printf("Root:");
    printNode(tree->root, 0);
    printf("\n");
}
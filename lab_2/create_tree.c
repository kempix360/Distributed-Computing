#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "tree.h"

int compareHash(int hash1, int hash2) {
    if (hash1 > hash2)
        return 1;
    else if (hash1 == hash2)
        return 0;
    else return -1;
}
int hashing_func(char* hash) {
    int result = 0;
    for (int i = 0; i < strlen(hash); i++) {
        result += (int)hash[i];
    }
    return result;
}

// Function to create a new ListNode
TreeNode* createTreeNode(char* key, char* value) {
    TreeNode* newNode = (TreeNode*)malloc(sizeof(TreeNode));
    int hash = hashing_func(key);
    if (newNode == NULL) {
        printf("Memory allocation failed.\n");
        exit(1);
    }
    newNode->value = value;
    newNode->key = key;
    newNode->color = 0;
    newNode->hash = hash;
    newNode->p = NULL;
    newNode->r = NULL;
    newNode->l = NULL;

    return newNode;
}

void rotateLeft(TreeNode* root, TreeNode* node) {
    TreeNode* node_2 = node->r;
    node->r = node_2->l;
    if (node_2->l != NULL) {
        node_2->l->p = node;
    }
    node_2->p = node->p;
    if (node->p == NULL) {
        root = node_2;
    } else if (node == node->p->l) {
        node->p->l = node_2;
    } else {
        node->p->r = node_2;
    }
    node_2->l = node;
    node->p = node_2;
}

void rotateRight(TreeNode* root, TreeNode* node) {
    TreeNode* node_2 = node->l;
    node->l = node_2->r;
    if (node_2->r != NULL) {
        node_2->r->p = node;
    }
    node_2->p = node->p;
    if (node->p == NULL) {
        root = node_2;
    } else if (node == node->p->r) {
        node->p->r = node_2;
    } else {
        node->p->l = node_2;
    }
    node_2->r = node;
    node->p = node_2;
}

void fixDoubleRed(TreeNode* root, TreeNode* x) {
    TreeNode* parent = NULL;
    TreeNode* grandparent = NULL;

    while ((x != root) && (x->color == 1) && (x->p->color == 1)) {
        parent = x->p;
        grandparent = x->p->p;

        if (parent == grandparent->l) {
            TreeNode* uncle = grandparent->r;

            if (uncle != NULL && uncle->color == 1) {
                grandparent->color = 1;
                parent->color = 0;
                uncle->color = 0;
                x = grandparent;
            } else {
                if (x == parent->r) {
                    rotateLeft(root, parent);
                    x = parent;
                    parent = x->p;
                }

                rotateRight(root, grandparent);
                int temp = parent->color;
                parent->color = grandparent->color;
                grandparent->color = temp;
                x = parent;
            }
        } else {
            TreeNode* uncle = grandparent->l;

            if ((uncle != NULL) && (uncle->color == 1)) {
                grandparent->color = 1;
                parent->color = 0;
                uncle->color = 0;
                x = grandparent;
            } else {
                if (x == parent->l) {
                    rotateRight(root, parent);
                    x = parent;
                    parent = x->p;
                }

                rotateLeft(root, grandparent);
                int temp = parent->color;
                parent->color = grandparent->color;
                grandparent->color = temp;
                x = parent;
            }
        }
    }
    root->color = 0;
}

void insertValue(char* key, char* value, RedBlackTree* tree) {
    int hash = hashing_func(key);

    TreeNode* new_node = createTreeNode(key, value);
    TreeNode* current = tree->root;
    TreeNode* prev = NULL;

    while (current != NULL) {
        int x = compareHash(hash, current->hash);
        if (x == 0){
            hash += 1;
            new_node->hash = hash;
            if (current != tree->root) {
                current = current->p;
            }
        }
        else if (x < 0) {
            prev = current;
            current = current->l;
        } else {
            prev = current;
            current = current->r;
        }
    }

    current = new_node;
    current->p = prev;

    if (prev == NULL) {
        tree->root = current;
    } else if (compareHash(current->hash, prev->hash) < 0) {
        prev->l = current;
    } else {
        prev->r = current;
    }

    current->color = 1;  // new node is red
    fixDoubleRed(tree->root, current);
}


TreeNode* searchForKey(TreeNode* root, char* key) {
    if (root == NULL || strcmp(root->key, key) == 0) {
        return root;
    }
    if (strcmp(key, root->key)<0) {
        return searchForKey(root->l, key);
    } else {
        return searchForKey(root->r, key);
    }
}

void getValue(char* key, RedBlackTree* tree) {
    //int hash = hashing_func(key);
    TreeNode* result = searchForKey(tree->root, key);

    if (result == NULL) {
        printf("Key not found in the tree.\n");
    } else {
        printf("Key: %s, Value: %s\n", key, result->value);
    }
}

void deleteValue(){

}

void clear_memory(TreeNode* root) {
    if (root == NULL) {
        return;
    }

    clear_memory(root->l);
    clear_memory(root->r);

    free(root->key);
    free(root->value);
    free(root);
}
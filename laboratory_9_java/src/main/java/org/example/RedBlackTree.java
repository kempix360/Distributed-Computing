package org.example;

import java.util.ArrayList;
import java.util.Objects;

public class RedBlackTree {
    TreeNode root;
    ArrayList<String> existing_keys;

    public static class TreeNode {
        String value;
        String key;
        int hash;
        int color;  // 1 for red, 0 for black
        TreeNode p;
        TreeNode r;
        TreeNode l;

        TreeNode(String key, String value) {
            this.key = key;
            this.value = value;
            this.hash = hashingFunc(key);
            this.color = 0; // Black by default
            this.p = null;
            this.r = null;
            this.l = null;
        }
    }

    public int compareHash(int hash1, int hash2) {
        if (hash1 > hash2)
            return 1;
        else if (hash1 == hash2)
            return 0;
        else
            return -1;
    }

    public static int hashingFunc(String hash) {
        int result = 0;
        for (int i = 0; i < hash.length(); i++) {
            result += (int) hash.charAt(i);
        }
        return result;
    }

    public TreeNode createTreeNode(String key, String value) {
        int hash = hashingFunc(key);
        return new TreeNode(key, value);
    }

    public void rotateLeft(TreeNode node) {
        TreeNode node_2 = node.r;
        node.r = node_2.l;
        if (node_2.l != null) {
            node_2.l.p = node;
        }
        node_2.p = node.p;
        if (node.p == null) {
            root = node_2;
        } else if (Objects.equals(node, node.p.l)) {
            node.p.l = node_2;
        } else {
            node.p.r = node_2;
        }
        node_2.l = node;
        node.p = node_2;
    }

    public void rotateRight(TreeNode node) {
        TreeNode node_2 = node.l;
        node.l = node_2.r;
        if (node_2.r != null) {
            node_2.r.p = node;
        }
        node_2.p = node.p;
        if (node.p == null) {
            root = node_2;
        } else if (Objects.equals(node, node.p.r)) {
            node.p.r = node_2;
        } else {
            node.p.l = node_2;
        }
        node_2.r = node;
        node.p = node_2;
    }

    public void fixDoubleRed(TreeNode x) {
        TreeNode parent = null;
        TreeNode grandparent = null;

        while ((x != root) && (x.color == 1) && (x.p.color == 1)) {
            parent = x.p;
            grandparent = x.p.p;

            if (Objects.equals(parent, grandparent.l)) {
                TreeNode uncle = grandparent.r;

                if (uncle != null && uncle.color == 1) {
                    grandparent.color = 1;
                    parent.color = 0;
                    uncle.color = 0;
                    x = grandparent;
                } else {
                    if (Objects.equals(x, parent.r)) {
                        rotateLeft(parent);
                        x = parent;
                        parent = x.p;
                    }

                    rotateRight(grandparent);
                    int temp = parent.color;
                    parent.color = grandparent.color;
                    grandparent.color = temp;
                    x = parent;
                }
            } else {
                TreeNode uncle = grandparent.l;

                if ((uncle != null) && (uncle.color == 1)) {
                    grandparent.color = 1;
                    parent.color = 0;
                    uncle.color = 0;
                    x = grandparent;
                } else {
                    if (Objects.equals(x, parent.l)) {
                        rotateRight(parent);
                        x = parent;
                        parent = x.p;
                    }

                    rotateLeft(grandparent);
                    int temp = parent.color;
                    parent.color = grandparent.color;
                    grandparent.color = temp;
                    x = parent;
                }
            }
        }
        root.color = 0;
    }

    public void insertValue(String key, String value) {
        int hash = hashingFunc(key);

        TreeNode new_node = createTreeNode(key, value);
        TreeNode current = root;
        TreeNode prev = null;

        while (current != null) {
            int x = compareHash(hash, current.hash);
            if (x == 0) {
                hash += 1;
                new_node.hash = hash;
                if (!Objects.equals(current, root)) {
                    current = current.p;
                }
            } else if (x < 0) {
                prev = current;
                current = current.l;
            } else {
                prev = current;
                current = current.r;
            }
        }

        new_node.p = prev;

        if (prev == null) {
            root = new_node;
        } else if (compareHash(new_node.hash, prev.hash) < 0) {
            prev.l = new_node;
        } else {
            prev.r = new_node;
        }

        new_node.color = 1;  // new node is red
        fixDoubleRed(new_node);

        if (existing_keys == null) {
            existing_keys = new ArrayList<>();
        }
        existing_keys.add(key);
    }

    public void printNode(TreeNode root, int level) {
        if (root == null) {
            return;
        }
        System.out.printf("(value: %s, key: %s, hash: %d, color: %s)\n", root.value, root.key, root.hash, root.color == 1 ? "red" : "black");

        if (root.l != null) {
            for (int i = 0; i < level; i++) {
                System.out.print("  ");
            }
            System.out.print("Left: ");
            printNode(root.l, level + 1);
        }

        if (root.r != null) {
            for (int i = 0; i < level; i++) {
                System.out.print("  ");
            }
            System.out.print("Right: ");
            printNode(root.r, level + 1);
        }
    }

    public void printTree() {
        System.out.print("Root:");
        printNode(root, 0);
        System.out.println();
    }

    public TreeNode searchForKey(String key) {
        if (existing_keys != null && existing_keys.contains(key)) {
            return root;
        }
        return null;
    }

    public boolean doesKeyAlreadyExist(String key) {
        return existing_keys != null && existing_keys.contains(key);
    }

}
package org.example;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.List;
import java.util.Queue;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;


public class Main {
    public static void main(String[] args) {
        Queue<String> queue = new LinkedList<>();
        RedBlackTree tree = new RedBlackTree();
        Object treeLock = new Object();

        String filename1 = "C:\\Users\\ArthurMorgan\\OneDrive\\Desktop\\4_semestr\\Przetwarzanie_rozproszone\\laboratory_9_java\\src\\main\\java\\org\\example\\file1.txt";
        String filename2 = "C:\\Users\\ArthurMorgan\\OneDrive\\Desktop\\4_semestr\\Przetwarzanie_rozproszone\\laboratory_9_java\\src\\main\\java\\org\\example\\file2.txt";

        Thread thread1 = new Thread(new KeyValueReader(filename1, queue, 1, tree, treeLock));
        Thread thread2 = new Thread(new KeyValueReader(filename2, queue, 2, tree, treeLock));

        thread1.start();
        thread2.start();

        new Thread(() -> {
            try {
                while (true) {
                    String line;
                    synchronized (queue) {
                        while (queue.isEmpty()) {
                            queue.wait(); // Oczekiwanie na nowe linie w kolejce
                        }
                        line = queue.poll();
                        queue.notifyAll(); // Powiadomienie o pobraniu linii z kolejki
                    }
                    if (line.equals("END")) {
                        break;
                    }
                    String[] parts = line.split(" ");
                    String key = parts[0];
                    String value = parts[1];
                    String threadId = parts[2];
                    System.out.println("Received key: " + key + ", value: " + value);
                    synchronized (treeLock) {
                        if (tree.doesKeyAlreadyExist(key)) {
                            System.out.println("THREAD: " + threadId + " \tNOT OK");
                        } else {
                            tree.insertValue(key, value);
                            System.out.println("THREAD: " + threadId + " \tOK");
                        }
                    }
                }
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
            synchronized (treeLock) {
                tree.printTree();
            }
        }).start();
    }
}

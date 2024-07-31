package org.example;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.ArrayList;
import java.util.List;
import java.util.Queue;
import java.util.concurrent.BlockingQueue;
import java.util.concurrent.LinkedBlockingQueue;

class KeyValueReader implements Runnable {
    private String filename;
    private final Queue<String> queue;
    private int threadId;

    public KeyValueReader(String filename, Queue<String> queue, int threadId, RedBlackTree tree, Object treeLock) {
        this.filename = filename;
        this.queue = queue;
        this.threadId = threadId;
    }

    @Override
    public void run() {
        try (BufferedReader br = new BufferedReader(new FileReader(filename))) {
            String line;
            while ((line = br.readLine()) != null) {
                line += " " + this.threadId;
                synchronized (queue) {
                    queue.add(line);
                    queue.notifyAll(); // Powiadomienie o nowej linii w kolejce
                }
                // Symulacja cyklicznego odczytu
                Thread.sleep(1000);
            }
            synchronized (queue) {
                queue.add("END");
                queue.notifyAll(); // Powiadomienie o zakończeniu pracy
            }
            System.out.println("THREAD: " + this.threadId + " \tended its work");
        } catch (IOException | InterruptedException e) {
            e.printStackTrace();
        }
    }
}


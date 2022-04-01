package algorithm.training.struct.list;

import java.util.LinkedList;
import java.util.Queue;
import java.util.Stack;

/**
 * @brief:      双向链表实现栈和队列
 *
 * */
public class D03_03_DoubleEndsQueueToStackAndQueue {
    public static class Node<T> {
        public T value;
        public Node<T> pre;
        public Node<T> next;

        public Node(T data) {
            value = data;
        }
    }

    public static class DoubleEndsQueue<T> {
        public Node head;
        public Node tail;

        public void addFromHead(T value) {
            if (head == null) {
                head = new Node(value);
                tail = head;
            } else {
                Node node = new Node(value);
                node.next = head;
                head.pre = node;
                head = node;
            }
        }

        public void addFromTail(T value) {
            if (tail == null) {
                tail = new Node(value);
                head = tail;
            } else {
                tail.next = new Node(value);
            }
        }

        public T popFromHead() {
            if (head == null) return null;

            Node<T> cur = head;
            if (head == tail) {
                head = null;
                tail = null;
            } else {
                head = head.next;
                head.pre = null;
                cur.next = null;
            }
            return cur.value;
        }


        public T popFromTail() {
            if (tail == null) return null;

            Node<T> cur = tail;
            if (head == tail) {
                head = null;
                tail = null;
            } else {
                tail = tail.pre;
                tail.next = null;
                cur.pre = null;
            }
            return cur.value;
        }

        public boolean isEmpty() {
            return head == null;
        }
    }

    public static class MyStack<T> {
        private DoubleEndsQueue<T> doubleEndsQueue;

        public MyStack() {
            doubleEndsQueue = new DoubleEndsQueue<>();
        }

        public void push(T value) {
            doubleEndsQueue.addFromHead(value);
        }

        public T pop() {
            return  doubleEndsQueue.popFromHead();
        }

        public boolean isEmpty() {
            return doubleEndsQueue.isEmpty();
        }
    }

    public static class MyQueue<T> {
        private DoubleEndsQueue<T> doubleEndsQueue;

        public MyQueue() {
            doubleEndsQueue = new DoubleEndsQueue<>();
        }

        public void push(T value) {
            doubleEndsQueue.addFromHead(value);
        }

        public T poll() {
            return doubleEndsQueue.popFromTail();
        }

        public boolean isEmpty() {
            return doubleEndsQueue.isEmpty();
        }
    }

    /************************** For Test ***************************/
    public static boolean isEqual(Integer o1, Integer o2) {
        if (o1 == null && o2 != null) {
            return false;
        }
        if (o1 != null && o2 == null) {
            return false;
        }
        if (o1 == null && o2 == null) {
            return true;
        }
        if (!o1.equals(o2)) {
            System.out.println("o1= " + o1 + " o2= " + o2);
        }
        return o1.equals(o2);
    }

    public static void main(String[] args) {
        int oneTestDataNum = 100;//100;
        int value = 10000;
        int testTimes = 100000;
        System.out.println("Test Start");
        for (int i = 0; i < testTimes; i++) {
            MyStack<Integer> myStack = new MyStack<>();
            MyQueue<Integer> myQueue = new MyQueue<>();
            Stack<Integer> stack = new Stack<>();
            Queue<Integer> queue = new LinkedList<>();
            for (int j = 0; j < oneTestDataNum; j++) {
                int nums = (int) (Math.random() * value);
                // System.out.print(nums + " ");
                if (stack.isEmpty()) {
                    myStack.push(nums);
                    stack.push(nums);
                } else {
                    if (Math.random() < 0.5) {
                        myStack.push(nums);
                        stack.push(nums);
                    } else {
                        if (!isEqual(myStack.pop(), stack.pop())) {
                            System.out.println("Stack oops!");
                        }
                    }
                }
                int numq = (int) (Math.random() * value);
                if (stack.isEmpty()) {
                    myQueue.push(numq);
                    queue.offer(numq);
                } else {
                    if (Math.random() < 0.5) {
                        myQueue.push(numq);
                        queue.offer(numq);
                    } else {
                        if (!isEqual(myQueue.poll(), queue.poll())) {
                            System.out.println("Queue oops!");
                        }
                    }
                }
            }
        }
        System.out.println("Test finish!");
    }
}

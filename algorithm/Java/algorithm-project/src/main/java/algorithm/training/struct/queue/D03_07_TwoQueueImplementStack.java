package algorithm.training.struct.queue;

import java.util.LinkedList;
import java.util.Queue;
import java.util.Stack;

/**
 * @brief:  队列实现栈
 *
 *          实现思想：
 *              方式1:利用两个队列outQueue进行数据查询和输出，另外一个helperQueue队列作辅助
 *                  假设push三个数字1, 2, 3
 *                      1. 初始化，将 1 push到helper,交换outQueue和helperQueue
 *                      2. 将2 push到helper，将outQueue元素取出push到helper，helper得到2 1,
 *                          交换outQueue和helperQueue
 *                      3  将3push到helper，将outQueue元素取出push到helper，helper得到3 2 1
 *
 *             方式2: 利用两个队列outQueue进行数据查询和输出，另外一个helperQueue队列作辅助
 *                  将所有的数据都push到outQueue
 *                  在数据查询pop、top时，弹出保存到helperQueue中，直到只剩下一个元素
 *
 *          https://leetcode-cn.com/problems/implement-stack-using-queues/
 * */
public class D03_07_TwoQueueImplementStack {
    public static class MyStack<T> {
        Queue<T>        outQueue;
        Queue<T>        helperQueue;

        public MyStack() {
            outQueue = new LinkedList<>();
            helperQueue = new LinkedList<>();
        }

        public void push(T x) {
            helperQueue.add(x);
            while (!outQueue.isEmpty()) {
                helperQueue.add(outQueue.poll());
            }
            Queue<T> tmp = helperQueue;
            helperQueue = outQueue;
            outQueue = tmp;
        }

        public T pop() {
            return outQueue.poll();
        }

        public T top() {
            return outQueue.peek();
        }

        public boolean empty() {
            return outQueue.isEmpty();
        }
    }

    public static class MyStack2<T> {
        Queue<T>        outQueue;
        Queue<T>        helperQueue;

        public MyStack2() {
            outQueue = new LinkedList<>();
            helperQueue = new LinkedList<>();
        }

        public void push(T x) {
            outQueue.add(x);
        }

        public T pop() {
            while (outQueue.size() > 1) {
                helperQueue.add(outQueue.poll());
            }
            T ans = outQueue.poll();
            // swap(outQueue, helperQueue);
            Queue<T> tmp = helperQueue;
            helperQueue = outQueue;
            outQueue = tmp;
            return ans;
        }

        public T top() {
            while (outQueue.size() > 1) {
                helperQueue.add(outQueue.poll());
            }
            T ans = outQueue.poll();
            helperQueue.add(ans);
            Queue<T> tmp = helperQueue;
            helperQueue = outQueue;
            outQueue = tmp;
            return ans;
        }

        public boolean empty() {
            return outQueue.isEmpty();
        }
    }

    public static void main(String[] args) {
        System.out.println("test begin");
        // MyStack2<Integer> myStack = new MyStack2();
        MyStack<Integer> myStack = new MyStack();
        Stack<Integer> test = new Stack<>();
        int testTime = 1000000;
        int max = 1000000;
        for (int i = 0; i < testTime; i++) {
            if (myStack.empty()) {
                if (!test.isEmpty()) {
                    System.out.println("Oops");
                }
                int num = (int) (Math.random() * max);
                myStack.push(num);
                test.push(num);
            } else {
                if (Math.random() < 0.25) {
                    int num = (int) (Math.random() * max);
                    myStack.push(num);
                    test.push(num);
                } else if (Math.random() < 0.5) {
                    if (!myStack.top().equals(test.peek())) {
                        System.out.println("Oops");
                    }
                } else if (Math.random() < 0.75) {
                    if (!myStack.pop().equals(test.pop())) {
                        System.out.println("Oops");
                    }
                } else {
                    if (myStack.empty() != test.isEmpty()) {
                        System.out.println("Oops");
                    }
                }
            }
        }

        System.out.println("test finish!");
    }
}

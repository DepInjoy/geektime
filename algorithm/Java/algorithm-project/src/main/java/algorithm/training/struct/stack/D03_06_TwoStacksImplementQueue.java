package algorithm.training.struct.stack;

import java.util.Stack;

/**
 * @brief:      利用栈结构实现队列
 *
 *              实现思想：
 *                  将两个栈串联在一起
 *
 *              https://leetcode-cn.com/problems/implement-queue-using-stacks/
 * */
public class D03_06_TwoStacksImplementQueue {
    class MyQueue {
        private Stack<Integer>      stack1;
        private Stack<Integer>      stack2;

        public MyQueue() {
            stack1 = new Stack<>();
            stack2 = new Stack<>();
        }

        public void push(int x) {
            while (!stack2.isEmpty()) {
                stack1.push(stack2.pop());
            }
            stack1.push(x);
            while (!stack1.isEmpty()) {
                stack2.push(stack1.pop());
            }
        }

        public int pop() {
            return stack2.pop();
        }

        public int peek() {
            return stack2.peek();
        }

        public boolean empty() {
            return stack2.isEmpty();
        }
    }
}

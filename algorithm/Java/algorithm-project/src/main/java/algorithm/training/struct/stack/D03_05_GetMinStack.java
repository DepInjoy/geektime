package algorithm.training.struct.stack;

import org.omg.PortableInterceptor.INACTIVE;

import java.util.Stack;

/**
 * @brief:  实现一个特殊的栈，在基本功能的基础上，再实现返回栈中最小元素的功能
 *              1. pop、push、getMin操作的时间复杂度都是 O(1)
 *              2. 设计的栈类型可以使用现成的栈结构
 *
 *
 * */
public class D03_05_GetMinStack {
    public static class MinStack {
        private Stack<Integer>      dataStack;
        private Stack<Integer>      minStack;

        public MinStack() {
            dataStack = new Stack<>();
            minStack = new Stack<>();
        }

        public int pop() {
            if (dataStack.isEmpty()) {
                throw new RuntimeException("Stack's empty, no data to pop");
            }

            if (minStack.peek().compareTo(dataStack.peek()) == 0) {
                minStack.pop();
            }
            return dataStack.pop();
        }

        public void push(int value) {
            dataStack.push(value);

            if (minStack.isEmpty() || value <= minStack.peek()) {
                minStack.push(value);
            }
        }

        public int top() {
            if (dataStack.isEmpty()) {
                throw new RuntimeException("Stack's empty, no data to pop");
            }

            return dataStack.peek();
        }

        public int getMin() {
            if (minStack.isEmpty()) {
                throw new RuntimeException("Stack's empty");
            }
            return minStack.peek();
        }
    }

    public static class MinStack2 {
        private Stack<Integer>      dataStack;
        private Stack<Integer>      minStack;

        public MinStack2() {
            dataStack = new Stack<>();
            minStack = new Stack<>();
        }

        public int pop() {
            if (dataStack.isEmpty()) {
                throw new RuntimeException("Stack's empty, no data to pop");
            }

            minStack.pop();
            return dataStack.pop();
        }

        public void push(int value) {
            dataStack.push(value);

            if (minStack.isEmpty() || value <= minStack.peek()) {
                minStack.push(value);
            } else {
                minStack.push(minStack.peek());
            }
        }

        public int top() {
            if (dataStack.isEmpty()) {
                throw new RuntimeException("Stack's empty, no data to pop");
            }

            return dataStack.peek();
        }

        public int getMin() {
            if (minStack.isEmpty()) {
                throw new RuntimeException("Stack's empty");
            }
            return minStack.peek();
        }
    }

    public static void main(String[] args) {
        MinStack minStack = new MinStack();
        // MinStack2 minStack = new MinStack2();
        minStack.push(5);
        System.out.println(minStack.getMin());
        minStack.push(4);
        System.out.println(minStack.getMin());
        minStack.push(1);
        System.out.println(minStack.getMin());
        minStack.push(3);
        System.out.println(minStack.getMin());
        minStack.push(2);
        minStack.push(1);
        System.out.println(minStack.getMin());
        minStack.pop();
        minStack.pop();
        minStack.pop();
        minStack.pop();
        System.out.println(minStack.getMin());
    }
}

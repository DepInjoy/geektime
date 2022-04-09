package algorithm.training.struct.stack;

import org.omg.PortableInterceptor.INACTIVE;

import java.util.Stack;

/**
 * @brief:  实现一个特殊的栈，在基本功能的基础上，再实现返回栈中最小元素的功能
 *              1. pop、push、getMin操作的时间复杂度都是 O(1)
 *              2. 设计的栈类型可以使用现成的栈结构
 *
 *          实现思想：
 *          用两个栈实现，存放最小元素的栈为minStack
 *              方式1. push时，minStack为空或者当前值<=minStack栈顶
 *                    则将数据push到minStack, 否则忽略测数据
 *              范式2. push时，minStack为空或者当前值<=minStack栈顶,
 *                    将当前值push到minStack，否则将栈顶元素再次压入minStack
 *
 *          https://leetcode-cn.com/problems/min-stack/
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

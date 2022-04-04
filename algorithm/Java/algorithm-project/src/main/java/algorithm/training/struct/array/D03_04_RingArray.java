package algorithm.training.struct.array;

import algorithm.practice.dc.week2.DC_02_05_SetAll;
import algorithm.training.struct.list.D03_03_DoubleEndsQueueToStackAndQueue;

/**
 * @brief:  用数组实现不超过固定大小的队列和栈
 *          队列用环形数组实现，栈正常使用
 *
 *          实现思想：
 *              队列数组的实现，维护头和尾两个索引, push从头插入，pop从尾弹出
 *              栈维护一个index索引，push后inde++，pop后index--
 * */

public class D03_04_RingArray {
    public static class MyQueue {
        private int[] array;
        private int headIndex;
        private int tailIndex;
        private int size;
        private int limit;

        public MyQueue(int limit) {
            array = new int[limit];
            headIndex = 0;
            tailIndex = 0;
            size = 0;
            this.limit = limit;
        }

        public void push(int value) {
            if (size == limit) {
                String errorMsg = "Queue's full, not allowed to push item";
                throw new RuntimeException(errorMsg);
            }
            size++;
            array[tailIndex] = value;
            tailIndex = nextIndex(tailIndex);
        }

        public int pop() {
            if (size == 0) {
                String errorMsg = "Queue's empty, not allowed to pop item";
                throw new RuntimeException(errorMsg);
            }

            size--;
            int ans = array[headIndex];
            headIndex = nextIndex(headIndex);
            return ans;
        }

        public boolean isEmpty() {
            return size == 0;
        }

        private int nextIndex(int index) {
            return index < limit - 1 ? index + 1 : 0;
        }
    }

    public static class MyStack {
        private int[] array;
        private int index;
        private int size;
        private int limit;

        public MyStack(int limit) {
            array = new int[limit];
            size = 0;
            index = -1;
            this.limit = limit;
        }

        public void push(int value) {
            if (size == limit) {
                throw new RuntimeException("Stack's full, not allowed to push item");
            }
            size++;
            index = (index < limit - 1 ? index + 1 : 0);
            array[index] = value;
        }

        public int pop() {
            if (size == 0) {
                throw new RuntimeException("Stack's empty, not allowed to push item");
            }

            size--;
            int ans = array[index];
            index = (index != 0 ? index - 1 : limit);
            return ans;
        }

        public boolean isEmpty() {
            return size == 0;
        }
    }

    public static void main(String[] args) {
        MyQueue mqueue = new MyQueue(5);
        assert mqueue.isEmpty();
        mqueue.push(0);
        mqueue.push(1);
        mqueue.push(2);
        mqueue.push(3);
        mqueue.push(4);
        try {
            mqueue.push(5);
        } catch (RuntimeException e) {
            System.out.println(e.getMessage());
        }

        assert mqueue.pop() == 0;
        assert mqueue.pop() == 1;
        assert mqueue.pop() == 2;
        assert mqueue.pop() == 3;
        assert mqueue.pop() == 4;
        try {
            mqueue.pop();
        } catch (RuntimeException e) {
            System.out.println(e.getMessage());
        }

        MyStack mstack = new MyStack(5);
        assert mstack.isEmpty();
        mstack.push(0);
        mstack.push(1);
        mstack.push(2);
        mstack.push(3);
        mstack.push(4);
        try {
            mstack.push(5);
        } catch (RuntimeException e) {
            System.out.println(e.getMessage());
        }

        assert mstack.pop() == 4;
        assert mstack.pop() == 3;
        assert mstack.pop() == 2;
        assert mstack.pop() == 1;
        assert mstack.pop() == 0;
        try {
            mstack.pop();
        } catch (RuntimeException e) {
            System.out.println(e.getMessage());
        }
    }
}

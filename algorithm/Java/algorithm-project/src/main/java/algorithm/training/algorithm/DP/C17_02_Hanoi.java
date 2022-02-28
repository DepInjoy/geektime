package algorithm.training.algorithm.DP;

import java.util.Stack;

/**
 * brief:               汉诺塔问题
 *              1. 递归实现
 *              2. 非递归实现
 * */
public class C17_02_Hanoi {
    public static void Hanoi1(int n) {
        if (n > 0) move(n, "left", "middle", "right");
    }

    /**
     *
     * */
    public static void Hanoi2(int N) {
        if (N < 1) return;

        Stack<Record> stack = new Stack<>();
        stack.push(new Record(N, false, "left", "middle", "right"));
        while (!stack.isEmpty()) {
            Record cur = stack.pop();
            if (cur.base == 1) {
                System.out.println("Move 1 from " + cur.from + " to " + cur.to);
                if (!stack.isEmpty()) {
                    stack.peek().finished = true;
                }
            } else {
                if (!cur.finished) { // 将N-1个盘子经过to转移到other, 腾地儿
                    stack.push(cur);
                    stack.push(new Record(cur.base - 1, false, cur.from, cur.to, cur.other));
                } else { // 将N-1个盘子从other经from转移到to
                    stack.push(new Record(cur.base - 1, false, cur.other, cur.from, cur.to));
                    System.out.println("Move " + cur.base +" from " + cur.from + " to " + cur.to);
                }
            }
        }
    }

    public static void move(int n, String from, String other, String to) {
        if (n == 1) {
            System.out.println("Move 1 from " + from + " to " + to);
        } else {
            move(n-1, from, to, other); // 将上面的N-1个转移到other为第N个盘子腾地
            System.out.println("Move " + n +" from " + from + " to " + to);
            move(n-1, other, from, to); // 第N个盘子转移走了,将剩下的N-1个盘子从other转移到目的地to
        }
    }

    public static class Record {
        public int      base;
        public boolean  finished;
        public String   from;
        public String   other;
        public String   to;

        public Record(int base, boolean finished, String from, String other, String to) {
            this.base = base;
            this.finished = finished;
            this.from = from;
            this.other = other;
            this.to = to;
        }
    }

    public static void main(String[] args) {
        Hanoi1(3);
        System.out.println();
        Hanoi2(3);
    }
}

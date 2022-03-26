package algorithm.training.algorithm.DP;

import java.util.ResourceBundle;
import java.util.Stack;

/**
 * @brief:      汉诺塔问题
 *
 * */
public class D17_02_Hanoi {
    public static void Hanoi1(int n) {
        if (n < 1) return;

        process(n, "left", "middle", "right");
    }

    public static class RecordInfo {
        public int level;
        public boolean finished;
        public String from;
        public String other;
        public String to;

        public RecordInfo(int level, boolean finished, String from, String other, String to) {
            this.level = level;
            this.finished = finished;
            this.from = from;
            this.other = other;
            this.to = to;
        }
    }

    // 非递归实现
    public static void Hanoi2(int n) {
        if (n < 1) return;

        Stack<RecordInfo> stack = new Stack<>();
        stack.push(new RecordInfo(n, false, "left", "middle", "right"));
        while (!stack.isEmpty()) {
            RecordInfo cur = stack.pop();
            if (cur.level == 1) {
                System.out.println("Move 1 from " + cur.from + " to " + cur.to);
                if (!stack.isEmpty()) {
                    stack.peek().finished = true;
                }
            } else {
                if (cur.finished) {
                    System.out.println("Move " + cur.level + " from " + cur.from + " to " + cur.to);
                    stack.push(new RecordInfo(cur.level - 1, false, cur.other, cur.from, cur.to));
                } else {
                    stack.push(cur);
                    stack.push(new RecordInfo(cur.level - 1, false, cur.from, cur.to, cur.other));
                }
            }
        }
    }


    public static void process(int n, String from, String other, String to) {
        if (n == 1) {
            System.out.println("Move 1 from " + from + " to " + to);
            return;
        }

        process(n-1, from, to, other);
        System.out.println("Move " + n + " from " + from + " to " + to);
        process(n-1, other, from, to);
    }

    public static void main(String[] args) {
        Hanoi1(3);
        System.out.println("非递归实现:");
        Hanoi2(3);
    }
}

package algorithm.training.algorithm.DP;

/**
 * brief:       汉诺塔问题
 * */
public class C17_02_Hanoi {
    public static void tryHanoi(int n) {
        if (n > 0) move(n, "left", "middle", "right");
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

    public static void main(String[] args) {
        tryHanoi(3);
    }
}

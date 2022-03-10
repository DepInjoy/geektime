package algorithm.practice.dc.week1;

/**
 * @brief:      给定一个非负整数num，不用循环语句，
 *              返回>=num，并且离num最近的，2的某次方
 *
 *              实现思想: 将所有的数字变成1再加1
 *              即 10101 -> 11111 -> 11111 + 1 = 100000
 * */
public class DC_01_03_Near2Power {
    public static final int tableSizeFor(int n) {
        n -= 1;     // 处理原本已经是n的某次方的数据
        n |= n >>> 1;
        n |= n >>> 2;
        n |= n >>> 4;
        n |= n >>> 8;
        n |= n >>> 16;
        return n < 0 ? 1 : n + 1;
    }

    public static void main(String[] args) {
        int n = 120;
        System.out.println(tableSizeFor(n));
    }
}

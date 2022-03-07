package algorithm.training.algorithm.DP;

/**
 * @brief:      N皇后问题是指在N*N的棋盘上要摆N个皇后，
 *              要求任何两个皇后不同行、不同列， 也不在同一条斜线上
 *              给定一个整数n，返回n皇后的摆法有多少种。n=1，返回1
 *              n=2或3，2皇后和3皇后问题无论怎么摆都不行，返回0
 *              n=8，返回92
 * */
public class D23_03_NQueens {
    public static int num1(int n) {
        return 0;
    }

    public static int num2(int n) {
        return 0;
    }

    public static void main(String[] args) {
        int n = 15;
        long start = System.currentTimeMillis();
        System.out.println(num2(n));
        long end = System.currentTimeMillis();
        System.out.println("cost time: " + (end - start) + "ms");

        start = System.currentTimeMillis();
        System.out.println(num1(n));
        end = System.currentTimeMillis();
        System.out.println("cost time: " + (end - start) + "ms");
    }
}

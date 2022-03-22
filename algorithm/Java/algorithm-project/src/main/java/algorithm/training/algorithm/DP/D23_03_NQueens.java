package algorithm.training.algorithm.DP;

/**
 * @brief:      N皇后问题是指在N*N的棋盘上要摆N个皇后，
 *              要求任何两个皇后不同行、不同列， 也不在同一条斜线上
 *              给定一个整数n，返回n皇后的摆法有多少种。n=1，返回1
 *              n=2或3，2皇后和3皇后问题无论怎么摆都不行，返回0
 *              n=8，返回92
 *
 *              https://leetcode-cn.com/problems/n-queens-ii/
 * */
public class D23_03_NQueens {
    public static int totalNQueens(int n) {
        if (n < 1) return 0;

        int[] record = new int[n];
        return process(record, 0, n);
    }

    // 当前来到row行，对n列尝试是否符合摆放条件
    // record记录了第row行皇后放在第record[row]列
    public static int process(int[] record, int row, int n) {
        if (row == n) return 1;

        int ways = 0;
        for (int j = 0; j < n; j++) {
            if (isValid(record, row, j)) {
                record[row] = j;
                ways += process(record, row + 1, n);
            }
        }
        return ways;
    }

    public static boolean isValid(int[] record, int row, int col) {
        for (int i = 0; i < row; i++) {
            if (record[i] == col || Math.abs(i - row) == Math.abs(record[i] - col)) {
                return false;
            }
        }
        return true;
    }

    public static int totalNQueens2(int n) {
        if (n < 1) return 0;


    }

    public static void main(String[] args) {
        int n = 8;
        long start = System.currentTimeMillis();
        System.out.println(num2(n));
        long end = System.currentTimeMillis();
        System.out.println("cost time: " + (end - start) + "ms");

        start = System.currentTimeMillis();
        System.out.println(totalNQueens(n));
        end = System.currentTimeMillis();
        System.out.println("cost time: " + (end - start) + "ms");
    }
}

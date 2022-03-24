package algorithm.training.algorithm.DP;

/**
 * @brief:      N皇后问题是指在N*N的棋盘上要摆N个皇后，
 *              要求任何两个皇后不同行、不同列， 也不在同一条斜线上
 *              给定一个整数n，返回n皇后的摆法有多少种。n=1，返回1
 *              n=2或3，2皇后和3皇后问题无论怎么摆都不行，返回0
 *              n=8，返回92
 *
 *              实现思想:
 *                  按行放置皇后(无需检测行冲突)，之后尝试在每列放置一个皇后，检测是否和之前摆放皇后的位置存在冲突
 *                  如果没有，记录皇后摆放的列信息
 *
 *              实现方式1: 将每行皇后摆放的皇后的列信息存储在int[] record中
 *              实现方式2: 用int limit取代record状态，通过减少常数项来降低算法的复杂度
 *                  假设对于7皇后问题，初始化limit为1 1 1 1 1 1 1
 *                  将参数初始化colLim, leftDiagLim, rightDiagLim都初始化为0，表示皇后可以存放在任意位置
 *                  相应位置为1，表示可以摆放皇后，为0表示无法摆放皇后
 *
 *                  那么假设在1行3列摆放了皇后，那么列下方、左右对角线无法摆放皇后，状态
 *                  列   0   1   2   3   4   5   6
 *                  0   1   1   1   1   1   1   1
 *                  1   1   1   0   1   1   1   1
 *                  2   1   0   0   0   1   1   1
 *                  3   1   1   1   1   1   1   1
 *                  4   1   1   1   1   1   1   1
 *                  5   1   1   1   1   1   1   1
 *                  6   1   1   1   1   1   1   1
 *
 *                  之后对2列可以摆放皇后的位置进行更新，由第一行可知0, 4, 5, 6可以尝试，假设摆放在6位置，状态会更新为
 *                  列   0   1   2   3   4   5   6
 *                  0   1   1   1   1   1   1   1
 *                  1   1   1   x   1   1   1   1
 *                  2   1   0   0   0   1   1   x
 *                  3   0   1   0   1   0   0   0
 *                  4   1   1   1   1   1   1   1
 *                  5   1   1   1   1   1   1   1
 *                  6   1   1   1   1   1   1   1
 *                  此时第三行可以尝试的位置便只剩下1, 3然后继续尝试
 *
 *              时间复杂度：O(N^N)
 *                  对于每行都有N中选择，一共N行，因此复杂度为O(N^N)
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

    // 不计算32皇后问题
    public static int totalNQueens2(int n) {
        if (n < 1 || n > 32) return 0;

        int limit = ((n == 32) ? -1 : (1 << n) - 1);
        return process(limit, 0, 0, 0);
    }

    // 之前皇后的列影响colLim
    // 之前皇后的左下对角线影响leftDiagLim
    // 之前皇后的右下对角线影响rightDiagLim
    public static int process(int limit, int colLim, int leftDiagLim, int rightDiagLim) {
        // 按行进行尝试，当colLim全是1表示每列都被正确放置了皇后
        if (colLim == limit) return 1;

        int ans = 0;
        int cur = limit & (~(colLim | leftDiagLim | rightDiagLim));
        while (cur != 0) {
            int mostRightOne = cur & (~cur + 1);
            cur = cur - mostRightOne;
            // 将列, 左对角线, 右对角线的影响延伸下去
            ans += process(limit, colLim | mostRightOne, (leftDiagLim | mostRightOne) << 1,
                    (rightDiagLim | mostRightOne) >>> 1);
        }
        return ans;
    }

    public static void main(String[] args) {
        int n = 14;
        long start = System.currentTimeMillis();
        System.out.println(totalNQueens2(n));
        long end = System.currentTimeMillis();
        System.out.println("cost time: " + (end - start) + "ms");

        start = System.currentTimeMillis();
        System.out.println(totalNQueens(n));
        end = System.currentTimeMillis();
        System.out.println("cost time: " + (end - start) + "ms");
    }
}

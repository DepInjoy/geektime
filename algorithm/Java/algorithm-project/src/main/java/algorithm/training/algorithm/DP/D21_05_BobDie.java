package algorithm.training.algorithm.DP;

/**
 * @brief:      给定5个参数，N，M，row，col，k
 *              表示在N*M的区域上，醉汉Bob初始在(row,col)位置
 *              Bob一共要迈出k步，且每步都会等概率向上下左右四个方向走一个单位
 *              任何时候Bob只要离开N*M的区域，就直接死亡
 *              返回k步之后，Bob还在N*M的区域的概率
 *
 *              数据填充过程
 *              当rest=0，棋牌中每个位置都为1
 *              之后，一层一层进行数据填充
 * */
public class D21_05_BobDie {
    public static double livePosibility1(int row, int col, int k, int N, int M) {
        if (row < 0 || row >= N || col < 0 || col >= M
                || k <= 0 || N <= 0 || M <= 0) return 0.0;

        return (double) process(row, col, k, N, M) / (Math.pow(4, k));
    }

    public static double livePosibility2(int row, int col, int k, int N, int M) {
        if (row < 0 || row >= N || col < 0 || col >= M
                || k <= 0 || N <= 0 || M <= 0) return 0.0;

        int [][][] dp = new int[N][M][k+1];
        for (int i = 0; i < N; i++) {
            for (int j = 0; j < M; j++) {
                dp[i][j][0] = 1;
            }
        }

        for (int rest = 1; rest <= k; rest++) {
            for (int i = 0; i < N; i++) {
                for (int j = 0; j < M; j++) {
                    dp[i][j][rest] = pick(dp, i + 1, j, rest - 1, N, M) +
                            pick(dp, i - 1, j, rest - 1, N, M) +
                            pick(dp, i, j + 1, rest - 1, N, M) +
                            pick(dp, i, j - 1, rest - 1, N, M);
                }
            }
        }
        return (double) dp[row][col][k] / (Math.pow(4, k));
    }

    public static int pick(int[][][] dp, int row, int col, int rest, int N, int M) {
        return (row < 0 || row >= N || col < 0 || col >= M) ? 0 : dp[row][col][rest];
    }

    // 返回Bob在(row,col)还有rest步,生还的次数
    public static int process(int row, int col, int rest, int N, int M) {
        if (row < 0 || row >= N || col < 0 || col >= M) return 0;
        if (rest == 0) return 1;
        return process(row + 1, col, rest - 1, N, M) +
                process(row - 1, col, rest - 1, N, M) +
                process(row, col + 1, rest - 1, N, M) +
                process(row, col - 1, rest - 1, N, M);
    }

    public static void main(String[] args) {
        System.out.println(livePosibility1(6, 6, 10, 50, 50));
        System.out.println(livePosibility2(6, 6, 10, 50, 50));
    }
}

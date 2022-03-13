package algorithm.practice.dc.week1;

/**
 * @brief:      LeetCode 329题: 给定一个二维数组matrix，
 *              你可以从任何位置出发，走向上下左右四个方向
 *              返回能走出来的最长的递增链长度
 *
 *              实现思想：
 *                  1. 对每个位置尝试，寻找最大值
 *                  2. 加缓存实现动态规划
 * */
public class DC_01_05_LongestIncreasingPath {
    public static int longestIncreasingPath1(int[][] matrix) {
        if (matrix == null || matrix.length == 0 || matrix[0].length == 0)
            return 0;

        int maxLength = 0;
        for (int i = 0; i < matrix.length; i++) {
            for (int j = 0; j < matrix[0].length; j++) {
                maxLength = Math.max(maxLength, process(matrix, i, j));
            }
        }
        return maxLength;
    }

    public static int process(int[][] matrix, int row, int col) {
        int M = matrix.length;
        int N = matrix[0].length;
        if (row < 0 || row >= M || col < 0 || col >= N) return -1;

        int maxWays = 1;
        if (col - 1 >= 0 && matrix[row][col] < matrix[row][col-1]) {
            maxWays = Math.max(process(matrix, row, col - 1) + 1, maxWays);
        }
        if (col + 1 < N && matrix[row][col] < matrix[row][col+1]) {
            maxWays = Math.max(process(matrix, row, col + 1) + 1, maxWays);
        }
        if (row - 1 >= 0 && matrix[row][col] < matrix[row-1][col]) {
            maxWays = Math.max(process(matrix, row-1, col) + 1, maxWays);
        }

        if (row + 1 < M && matrix[row][col] < matrix[row+1][col]) {
            maxWays = Math.max(process(matrix, row+1, col) + 1, maxWays);
        }
        return maxWays;
    }

    // 加缓存实现动态规划优化
    public int longestIncreasingPath2(int[][] matrix) {
        if (matrix == null || matrix.length == 0 || matrix[0].length == 0)
            return 0;

        int M = matrix.length;
        int N = matrix[0].length;
        int [][] dp = new int[M][N];
        int maxLength = 0;
        for (int i = 0; i < matrix.length; i++) {
            for (int j = 0; j < matrix[0].length; j++) {
                maxLength = Math.max(maxLength, process(matrix, i, j, dp));
            }
        }
        return maxLength;
    }

    public int process(int[][] matrix, int row, int col, int[][] dp) {
        if (dp[row][col] != 0) return dp[row][col];

        int M = matrix.length;
        int N = matrix[0].length;
        int maxWays = 1;
        if (col - 1 >= 0 && matrix[row][col] < matrix[row][col-1]) {
            maxWays = Math.max(process(matrix, row, col - 1, dp) + 1, maxWays);
        }
        if (col + 1 < N && matrix[row][col] < matrix[row][col+1]) {
            maxWays = Math.max(process(matrix, row, col + 1, dp) + 1, maxWays);
        }
        if (row - 1 >= 0 && matrix[row][col] < matrix[row-1][col]) {
            maxWays = Math.max(process(matrix, row-1, col, dp) + 1, maxWays);
        }

        if (row + 1 < M && matrix[row][col] < matrix[row+1][col]) {
            maxWays = Math.max(process(matrix, row+1, col, dp) + 1, maxWays);
        }
        dp[row][col] = maxWays;
        return maxWays;
    }
}

package algorithm.training.algorithm.DP;

/**
 * @brief:      给定一个二维数组matrix，一个人必须从左上角出发，最后到达右下角
 *              沿途只可以向下或者向右走，沿途的数字都累加就是距离累加和
 *              返回最小距离累加和
 *
 *              对于5*5的输入有
 *              i   i   i   i   i       1. 只能是从左到右，可直接初始化
 *              i   x   x
 *              i
 *              i                       2. 公式从做到右，从上到下进行填充
 *              i               x       返回值
 *               2. 只能是从上到下，可直接初始化
 *
 *              由于动态规划过程中只依赖上面一行，因此可以进行空间压缩
 *              如果要对空间压缩的实现进行进一步的优化，可以对行/列小的进行数据缓存，节省空间
 *              空间压缩适合于如下情形
 *              a           a   c       a   c
 *              b   x       b   x           x
 * */
public class D21_01_MinPathSum {
    public static int minPathSum1(int[][] m) {
        if (m == null || m.length == 0 || m[0] == null || m[0].length == 0) return -1;

        int M = m.length - 1;
        int N = m[0].length - 1;
        return process(m, M, N);
    }

    public static int process(int[][] m, int row, int col) {
        int M = m.length - 1;
        int N = m[0].length - 1;
        if (row < 0 || row > M || col < 0 || col > N)
            return Integer.MAX_VALUE;

        if (row == 0 && col == 0)
            return m[row][col];

        int p1 = process(m, row - 1, col);
        int p2 = process(m, row, col - 1);
        return Math.min(p1, p2) + m[row][col];
    }

    // 动态规划
    public static int minPathSum2(int[][] m) {
        if (m == null || m.length == 0 || m[0].length == 0) return -1;

        int M = m.length - 1;
        int N = m[0].length - 1;
        int[][] dp = new int[M+1][N+1];
        dp[0][0] = m[0][0];
        for(int row = 1; row <= M; row++) {
            dp[row][0] = dp[row-1][0] + m[row][0];
        }
        for (int col = 1; col <= N; col++) {
            dp[0][col] = dp[0][col-1] + m[0][col];
        }
        for (int row = 1; row <= M; row++) {
            for (int col = 1; col <= N; col++) {
                dp[row][col] = Math.min(dp[row-1][col], dp[row][col-1]) + m[row][col];
            }
        }
        return dp[M][N];
    }

    // 动态规划+空间压缩
    public static int minPathSum3(int[][] m) {
        if (m == null || m.length == 0 || m[0].length == 0) return -1;

        int M = m.length - 1;
        int N = m[0].length - 1;
        int[] dp = new int[N+1];
        dp[0] = m[0][0];
        for(int col = 1; col <= N; col++) {
            dp[col] = dp[col-1] + m[0][col];
        }
        for(int row = 1; row <= M; row ++) {
            dp[0] = dp[0] + m[row][0];
            for (int col = 1; col <= N; col++) {
                dp[col] = Math.min(dp[col-1], dp[col]) + m[row][col];
            }
        }
        return dp[N];
    }

    /**************************** For Test *************************/
    public static int[][] generateRandomMatrix(int rowSize, int colSize) {
        if (rowSize < 0 || colSize < 0) {
            return null;
        }
        int[][] result = new int[rowSize][colSize];
        for (int i = 0; i != result.length; i++) {
            for (int j = 0; j != result[0].length; j++) {
                result[i][j] = (int) (Math.random() * 100);
            }
        }
        return result;
    }

    public static void printMatrix(int[][] matrix) {
        for (int i = 0; i != matrix.length; i++) {
            for (int j = 0; j != matrix[0].length; j++) {
                System.out.print(matrix[i][j] + " ");
            }
            System.out.println();
        }
    }

    public static void main(String[] args) {
        int rowSize = 10;
        int colSize = 10;
        int[][] m = generateRandomMatrix(rowSize, colSize);
        System.out.println(minPathSum1(m));
        System.out.println(minPathSum2(m));
        System.out.println(minPathSum3(m));
    }
}

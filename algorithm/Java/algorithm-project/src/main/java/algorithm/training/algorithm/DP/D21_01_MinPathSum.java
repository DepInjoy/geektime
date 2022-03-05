package algorithm.training.algorithm.DP;

/**
 * @brief:      给定一个二维数组matrix，一个人必须从左上角出发，最后到达右下角
 *              沿途只可以向下或者向右走，沿途的数字都累加就是距离累加和
 *              返回最小距离累加和
 * */
public class D21_01_MinPathSum {
    public static int minPathSum1(int[][] m) {
        return 0;
    }

    public static int minPathSum2(int[][] m) {
        return 0;
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
    }
}

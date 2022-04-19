package algorithm.practice.dc.week3;
/**
 * @brief:  给定一个只有0和1组成的二维数组, 返回边框全是1的最大正方形面积
 *          https://leetcode-cn.com/problems/closest-subsequence-sum/
 * */
public class L03_03_Largest1BorderedSquare {
    int largest1BorderedSquare(int[][] grid) {
        if (grid == null || grid.length == 0 || grid[0].length == 0) return 0;

        int M = grid.length, N = grid[0].length;
        int[][] right = new int[M][N];
        int[][] down = new int[M][N];
        setBorderedInfo(grid, right, down);
        for (int size = Math.min(M, N); size >= 0; size--) {
            if (isBorderedSquare(size, right, down)) {
                return size * size;
            }
        }
        return 0;
    }

    void setBorderedInfo(int[][] grid, int[][] right, int[][] down) {
        int M = grid.length, N = grid[0].length;
        if (grid[M-1][N-1] == 1) {
            right[M-1][N-1] = 1;
            down[M-1][N-1] = 1;
        }

        for (int i = N - 2; i >= 0; i--) {
            if (grid[M-1][i] == 1) {
                right[M-1][i] = right[M-1][i+1] + 1;
                down[M-1][i] = 1;
            }
        }

        for (int i = M - 2; i >= 0; i--) {
            if (grid[i][N-1] == 1) {
                right[i][N-1] = 1;
                down[i][N-1] = down[i+1][N-1] + 1;
            }
        }

        for (int i = M-2; i >= 0; i--) {
            for (int j = N-2; j >= 0; j--) {
                if (grid[i][j] == 1) {
                    right[i][j] = right[i][j+1] + 1;
                    down[i][j] = down[i+1][j] + 1;
                }
            }
        }
    }

    boolean isBorderedSquare(int size, int[][] right, int[][] down) {
        for (int i = 0; i < right.length; i++) {
            for (int j = 0; j < right[0].length; j++) {
                if (right[i][j] >= size && down[i][j] >= size && right[i+size][j] >= size && down[i][j+size] >= size) {
                    return true;
                }
            }
        }
        return false;
    }
}

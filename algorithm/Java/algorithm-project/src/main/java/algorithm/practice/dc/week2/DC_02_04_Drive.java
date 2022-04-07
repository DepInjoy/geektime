package algorithm.practice.dc.week2;

import java.util.Arrays;

/**
 * @brief:      现有司机N*2人，调度中心会将所有司机平分给A、B两个区域
 *              第i个司机去A可得收入为income[i][0]，
 *              第i个司机去B可得收入为income[i][1]，
 *              返回所有调度方案中能使所有司机总收入最高的方案，是多少钱
 *
 *              实现思想：
 *                  1. 暴力尝试，对于第i个可以选择去A区或B区，且A区需要去N个人，A区剩下rest个名额
 *                  2. 记忆化搜索加动态规划
 *                  3. 贪心实现，假设先让所有司机去A，得到一个总收益，然后让N个司机改换去B，可以获得最大的额外收益
 * */
public class DC_02_04_Drive {
    // 暴力尝试
    public static int maxMoney1(int[][] income) {
        if (income == null || income.length < 2 || (income.length & 0x1) != 0) return 0;
        return process(income, 0, income.length / 2);
    }

    public static int process(int[][] income, int index, int rest) {
        int N = income.length;
        if (index == N) {
            return rest == 0 ? 0 : Integer.MIN_VALUE;
        }

        if (rest == 0) {
            return income[index][1] + process(income, index + 1, rest);
        }

        if (N - index == rest) {
            return income[index][0] + process(income, index + 1, rest - 1);
        }

        return Math.max(income[index][0] + process(income, index + 1, rest - 1),
                income[index][1] + process(income, index + 1, rest));
    }

    // 动态规划
    public static int maxMoney2(int[][] income) {
        if (income == null || income.length < 2 || (income.length & 0x1) != 0) return 0;

        int N = income.length;
        int M = N >> 1;
        int[][] dp = new int[N + 1][M + 1];
        dp[N][0] = 0;
        for (int i = 1; i <= M; i++) {
            dp[N][i] = Integer.MIN_VALUE;
        }

        for (int index = N - 1; index >= 0; index--) {
            for (int rest = 0; rest <= M; rest++) {
                if (rest == 0) {
                    dp[index][rest] = income[index][1] + dp[index + 1][rest];
                } else if (N - index == rest) {
                    dp[index][rest] = income[index][0] + dp[index + 1][rest - 1];
                } else {
                    dp[index][rest] = Math.max(income[index][0] + dp[index + 1][rest - 1],
                            income[index][1] + dp[index + 1][rest]);
                }
            }
        }
        return dp[0][M];
    }

    // 贪心实现
    public static int maxMoney3(int[][] income) {
        if (income == null || income.length < 2 || (income.length & 0x1) != 0) return 0;

        int N = income.length;
        int M = N >> 1;
        int sum = 0;
        int[] diff = new int[N];
        for (int i = 0; i < N; i++) {
            diff[i] = income[i][1] - income[i][0];
            sum += income[i][0];
        }
        Arrays.sort(diff);
        for (int i = M; i < N; i++) {
            sum += diff[i];
        }
        return sum;
    }

    /**************************** For Test ********************/
    // 返回随机len*2大小的正数矩阵,值在0~value-1之间
    public static int[][] randomMatrix(int len, int value) {
        int[][] ans = new int[len << 1][2];
        for (int i = 0; i < ans.length; i++) {
            ans[i][0] = (int) (Math.random() * value);
            ans[i][1] = (int) (Math.random() * value);
        }
        return ans;
    }

    public static void printMatrix(int[][] matrix) {
        for (int i = 0; i < matrix.length; i++) {
            for (int j = 0; j < matrix[0].length; j++) {
                System.out.print(matrix[i][j] + " ");
            }
            System.out.println();
        }
        System.out.println();
    }

    public static void main(String[] args) {
        int N = 10;
        int value = 100;
        int testTime = 50000;
        System.out.println("测试开始");
        for (int i = 0; i < testTime; i++) {
            int len = (int) (Math.random() * N) + 1;
            int[][] matrix = randomMatrix(len, value);
            int ans1 = maxMoney1(matrix);
            int ans2 = maxMoney2(matrix);
            int ans3 = maxMoney3(matrix);
            if (ans1 != ans2 || ans1 != ans3) {
                printMatrix(matrix);
                System.out.println(ans1);
                System.out.println(ans2);
                System.out.println(ans3);
                System.out.println("Oops!");
                break;
            }
        }
        System.out.println("测试结束");
    }
}

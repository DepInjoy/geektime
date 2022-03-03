package algorithm.training.algorithm.DP;

/**
 * @brief:      给定两个长度都为N的数组weights和values，
 *              weights[i]和values[i]分别代表 i号物品的重量和价值。
 *              给定一个正数bag，表示一个载重bag的袋子，你装的物品不能超过这个重量。
 *              返回你能装下最多的价值是多少?
 *
 *              数据填充过程
 *              weights = { 3, 2, 4, 7, 3, 1}
 *              values = { 5, 6, 3, 19, 12, 4};
 *              bag = 5;
 *
 *                  0   1   2   3   4   5
 *              0
 *              1
 *              2
 *              3
 *              4                                   2. dp[i][rest] = Math.max(v1, v2) 不超重情况下要i和不要i的最大值
 *              5
 *              6   0   0   0   0   0   0           1. dp[N][...] = 0
 * */
public class D19_01_Knapsack {
    public static int maxValue(int[] w, int[] v, int bag) {
        if (w == null || v == null || w.length != v.length || w.length == 0 || bag < 0) {
            return 0;
        }
        return process(w, v, 0, bag);
    }

    public static int process(int[] w, int[] v, int index, int rest) {
        if (rest < 0) return -1;
        if (index == w.length) return 0;

        int v2 = process(w, v, index + 1, rest);
        int v1 = process(w, v, index + 1, rest - w[index]);
        if (v1 != -1) v1 += v[index];
        return Math.max(v1, v2);
    }

    public static int maxValue2(int[] w, int[] v, int bag) {
        if (w == null || v == null || w.length != v.length || w.length == 0 || bag < 0) {
            return 0;
        }

        int[][] dp = new int[w.length+1][bag + 1];
        // dp[w.length][...] = 0;
        for (int i = w.length - 1; i >= 0; i--) {
            for (int rest = 0; rest <= bag; rest++) {
                int v1 = dp[i+1][rest];
                int v2 = 0;
                if (rest >= w[i]) {
                    v2 = dp[i+1][rest-w[i]] + v[i];
                }
                dp[i][rest] = Math.max(v1, v2);
            }
        }
        return dp[0][bag];
    }


    public static void main(String[] args) {
        int[] weights = { 3, 2, 4, 7, 3, 1, 7 };
        int[] values = { 5, 6, 3, 19, 12, 4, 2 };
        int bag = 15;
        System.out.println(maxValue(weights, values, bag));
        System.out.println(maxValue2(weights, values, bag));
    }
}

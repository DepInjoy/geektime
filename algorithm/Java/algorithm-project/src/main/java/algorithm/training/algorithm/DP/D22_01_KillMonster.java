package algorithm.training.algorithm.DP;

import java.util.HashMap;

/**
 * @brief:      给定3个参数，N，M，K, 怪兽有N滴血，等着英雄来砍自己
 *              英雄每一次打击，都会让怪兽流失[0~M]的血量
 *              到底流失多少？每一次在[0~M]上等概率的获得一个值
 *              求K次打击之后，英雄把怪兽砍死的概率
 *
 *              实现思想：
 *                  每次砍的可能取值是(M+1)中可能，一共砍K次
 *
 *                  1. 暴力递归
 *                  2. 动态规划
 *                  3. 动态规划+斜率优化
 * */
public class D22_01_KillMonster {
    // 暴力递归
    public static double right(int N, int M, int K) {
        if (N <= 0 || M <= 0 || K <= 0) return 0;
        return (double)process(M, K, N) / Math.pow(M+1, K);
    }

    public static long process(int M, int time, int rest) {
        if (rest <= 0) return (long)Math.pow(M+1, time);
        if (time == 0) return 0;

        int ans = 0;
        for (int i = 0; i <= M; i++) {
            ans += process(M, time-1, rest - i);
        }
        return ans;
    }

    public static double dp1(int N, int M, int K) {
        if (N <= 0 || M <= 0 || K <= 0) return 0;

        long[][] dp = new long[K+1][N+1];
        dp[0][0] = 1;
        for (int time = 1; time <= K;time++) {
            dp[time][0] = (long)Math.pow(M+1, time);
            for (int rest = 1; rest <= N; rest++) {
                long ways = 0;
                for (int m = 0; m <= M; m++) {
                    if (rest <= m) {
                        ways += (long)Math.pow(M+1, time-1);
                    } else {
                        ways += dp[time-1][rest-m];
                    }
                }
                dp[time][rest] = ways;
            }
        }
        return dp[K][N] / Math.pow(M+1, K);
    }

    // 动态规划
    public static long process(int M, int time, int rest, HashMap<Long, Long> hashMap) {
        if (rest <= 0) return (long) Math.pow(M+1, time);
        if (time == 0) return 0;
        if (hashMap.containsKey(time)) {
            return hashMap.get(time);
        }

        long ans = 0;
        for (int i = 0; i <= M; i++) {
            ans += process(M, time-1, rest-i, hashMap);
        }
        hashMap.put((long)time, ans);
        return ans;
    }

    // 动态规划+斜率优化
    public static double dp2(int N, int M, int K) {
        if (N <= 0 || M <= 0 || K <= 0) return 0;

        long[][] dp = new long[K+1][N+1];
        dp[0][0] = 1;
        for (int time = 1; time <= K;time++) {
            dp[time][0] = (long)Math.pow(M+1, time);
            for (int rest = 1; rest <= N; rest++) {
                dp[time][rest] = dp[time][rest-1] + dp[time-1][rest];
                if (rest - 1 - M >= 0) {
                    dp[time][rest] -= dp[time-1][rest - 1 - M];
                } else {
                    dp[time][rest] -= Math.pow(M+1, time-1);
                }
            }
        }
        return dp[K][N] / Math.pow(M+1, K);
    }

    /****************** For Test *************************/
    public static void main(String[] args) {
        int NMax = 10;
        int MMax = 10;
        int KMax = 10;
        int testTime = 200;
        System.out.println("测试开始");
        for (int i = 0; i < testTime; i++) {
            int N = (int) (Math.random() * NMax);
            int M = (int) (Math.random() * MMax);
            int K = (int) (Math.random() * KMax);
            double ans1 = right(N, M, K);
            double ans2 = dp1(N, M, K);
            double ans3 = dp2(N, M, K);
            if (ans1 != ans2 || ans1 != ans3) {
                System.out.println("Oops!");
                System.out.println("N= " + M + " M= " + M + " K= " + K);
                break;
            }
        }
        System.out.println("测试结束");
    }
}

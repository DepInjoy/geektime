package algorithm.training.algorithm.DP;

/**
 * @brief:      给定一个字符串str，返回这个字符串的最长回文子序列长度
 *              比如 ： str = “a12b3c43def2ghi1kpm”
 *              最长回文子序列是“1234321”或者“123c321”，返回长度7
 *
 *              bbbab
 *              L R 0   1   2   3   4
 *              0   1   2
 *              1       1   2   x2  x3
 *              2           1   1   x1
 *              3               1   1
 *              4                   1
 *
 *              https://leetcode-cn.com/problems/longest-palindromic-subsequence/
 * */
public class D20_01_PalindromeSubsequence {
    public static int longestPalindromeSubseq(String s) {
        if (s == null || s.length() == 0) {
            return 0;
        }

        char[] str = s.toCharArray();
        return process(str, 0, str.length - 1);
    }

    public static int process(char[] str, int L, int R) {
        if (L == R) return 1;
        if (L == R - 1) return str[L] == str[R] ? 2 : 1;

        int p1 = process(str, L + 1, R - 1);
        int p2 = process(str, L + 1, R);
        int p3 = process(str, L,  R - 1);
        int p4 = str[L] == str[R] ? 2 + process(str, L + 1, R - 1) : 0;
        return Math.max(Math.max(p1, p2), Math.max(p3, p4));
    }

    public static int longestPalindromeSubseq1(String s) {
        if (s == null || s.length() == 0) {
            return 0;
        }

        char[] str = s.toCharArray();
        int N = str.length;
        int[][] dp = new int[N][N];
        dp[N-1][N-1] = 1;
        for (int i = 0; i < N-1; i++) {
            dp[i][i] = 1;
            dp[i][i+1] = str[i] == str[i+1] ? 2 : 1;
        }

        for (int i = N - 3; i >= 0; i--) {
            for (int j = i + 2; j < N; j++) {
                int p1 = dp[i+1][j-1];
                int p4 = str[i] == str[j] ? 2 + dp[i+1][j-1] : 0;
                dp[i][j] = Math.max(Math.max(dp[i+1][j-1], dp[i+1][j]),
                        Math.max(dp[i][j-1], p4));
            }
        }
        return dp[0][N-1];
    }
}

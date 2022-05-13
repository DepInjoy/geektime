/**
 * @brief:  516. 最长回文子序列
 *          给你一个字符串 s ，找出其中最长的回文子序列，并返回该序列的长度。
 *          子序列定义为：不改变剩余字符顺序的情况下，删除某些字符或者不删除任何字符形成的一个序列。
 *          链接：https://leetcode.cn/problems/longest-palindromic-subsequence
 * 
 *          实现思想：
 *              dp[i][j]表示从i到j最长回文子序列的长度
 *              状态转移方程：dp[i][j] = dp[i+1][j-1] + 2                s[i]==s[j]
 *                                      max(dp[i+1][j], dp[i][j-1])     s[i]!=s[j]
 * */

int longestPalindromeSubseq(string s) {
    int n = s.size();
    vector<vector<int>> dp(n, vector<int>(n));
    for (int i = n-1; i >= 0; i--) {
        dp[i][i] = 1;
        for (int j = i+1; j < n; j++) {
            if (s[i] == s[j]) {
                dp[i][j] = dp[i+1][j-1] + 2;
            } else {
                dp[i][j] = max(dp[i+1][j], dp[i][j-1]);
            }
        }
    }
    return dp[0][n-1];
}
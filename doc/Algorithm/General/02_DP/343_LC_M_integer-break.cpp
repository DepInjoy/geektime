/**
 * 343. 整数拆分
 * https://leetcode.cn/problems/integer-break/description/
*/

// 时间复杂度O(N^2), 空间复杂度O(N)
// dp[i]= max{max(j*(i-j), j*dp[i-j])},其中1 <= j < i
int integerBreak(int n) {
    std::vector<int> dp(n+1);
    for (int i = 2; i <= n; ++i) {
        int cur_max = 0;
        for (int j = 1; j < i; ++j) {
            cur_max = std::max(cur_max, std::max(j * (i - j), j * dp[i-j]));
        }
        dp[i] = cur_max;
    }
    return dp[n];
}


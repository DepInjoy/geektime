/**
 * @brief   343. 整数拆分
 *          给定一个正整数 n ，将其拆分为 k 个 正整数 的和（ k >= 2 ），
 *          并使这些整数的乘积最大化。返回 你可以获得的最大乘积 。
 *          链接：https://leetcode.cn/problems/integer-break/
 * 
 *          实现思想：
 *              1. 动态规划, 时间复杂度O(N^2), 空间复杂度O(N)
 *              2. 优化动态规划,时间复杂度O(N), 空间复杂度O(N)
 *                  dp[i]=max(2×(i−2),2×dp[i−2],3×(i−3),3×dp[i−3])
 *              3. 数学
 */

// 动态规划
int integerBreak(int n) {
    vector<int> dp(n+1);
    dp[1] = 1;
    for (int i = 2; i <= n; i++) {
        int curMax = 0;
        for (int j = 1; j < i; j++) {
            curMax = max(curMax, max(j * (i-j), j * dp[i-j]));
        }
        dp[i] = curMax;
    }
    return dp[n];
}

// 优化动态规划
int integerBreak(int n) {
    vector<int> dp(n+1);
    dp[1] = 1, dp[2] = 1;
    for (int i = 3; i <= n; i++) {
        dp[i] = max(max(2*(i-2), 2*dp[i-2]), max(3*(i-3), 3* dp[i-3]));
    }
    return dp[n];
}
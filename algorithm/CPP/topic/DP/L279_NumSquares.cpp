/**
 * @brief    给你一个整数 n ，返回 和为 n 的完全平方数的最少数量
 *          链接：https://leetcode.cn/problems/perfect-squares/
 *          实现思想:
 *              1. 动态规划
 *              2. 数学，四平方和定理
 */
int numSquares(int n) {
    vector<int> dp(n+1);
    for (int i = 1; i <= n; i++) {
        int curMin = INT_MAX;
        for (int j = 1; j*j <= i; j++) {
            curMin = min(curMin, dp[i-j*j]);
        }
        dp[i] = curMin + 1;
    }
    return dp[n];
}
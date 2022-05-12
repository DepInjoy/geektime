/**
 * @brief   64. 最小路径和
 *          给定一个包含非负整数的mxn网格grid，请找出一条从左上角到右下角的路径，
 *          使得路径上的数字总和为最小。
 *          说明：每次只能向下或者向右移动一步。
 *          链接：https://leetcode.cn/problems/minimum-path-sum/
 * 
 *          实现思想：
 *              1. 动态规划+空间压缩
 */

// 动态规划+空间压缩
int minPathSum(vector<vector<int>>& grid) {
    int m = grid.size(), n = grid[0].size();
    vector<int> dp(n);
    dp[0] = grid[0][0];
    for (int i = 1; i < n; i++) dp[i] = grid[0][i] + dp[i-1];
    for (int i = 1; i < m; i++) {
        dp[0] += grid[i][0];
        for (int j = 1; j < n; j++) {
            dp[j] = min(dp[j], dp[j-1]) + grid[i][j];
        }
    }
    return dp[n-1];
}
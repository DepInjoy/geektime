/**
 * @brief   63. 不同路径 II
 *          一个机器人位于一个mxn网格的左上角(起始点在下图中标记为 “Start”)。
 *          机器人每次只能向下或者向右移动一步。机器人试图达到网格的右下角(在下图中标记为“Finish”)
 *          现在考虑网格中有障碍物。那么从左上角到右下角将会有多少条不同的路径？
 *          网格中的障碍物和空位置分别用 1 和 0 来表示。
 *          链接：https://leetcode.cn/problems/unique-paths-ii
 * 
 *          实现思想：
 *              1. 动态规划
 *              2. 动态规划+空间压缩(滚动数组)
 * 
 */

// 动态规划
int uniquePathsWithObstacles(vector<vector<int>>& obstacleGrid) {
    int m = obstacleGrid.size(), n = obstacleGrid[0].size();
    vector<vector<int>> dp(m, vector<int>(n));
    dp[0][0] = obstacleGrid[0][0] ? 0 : 1;
    for (int i = 1; i < m; i++) dp[i][0] = ((obstacleGrid[i][0] || !dp[i-1][0]) ? 0 : 1);
    for (int i = 1; i < n; i++) dp[0][i] = ((obstacleGrid[0][i] || !dp[0][i-1]) ? 0 : 1);
    for (int i = 1; i < m; i++) {
        for (int j = 1; j < n; j++) {
            if (obstacleGrid[i][j]) {
                dp[i][j] = 0;
            } else {
                dp[i][j] = dp[i-1][j] + dp[i][j-1];
            }
        }
    }
    return dp[m-1][n-1];
}

// 动态规划+空间压缩(滚动数组)
int uniquePathsWithObstacles(vector<vector<int>>& obstacleGrid) {
    int m = obstacleGrid.size(), n = obstacleGrid[0].size();
    vector<int> dp(n);
    dp[0] = obstacleGrid[0][0] ? 0 : 1;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (obstacleGrid[i][j]) {
                dp[j] = 0;
            } else if (j-1>=0 && !obstacleGrid[i][j-1]) {
                dp[j] += dp[j-1];
            }
        }
    }
    return dp[n-1];
}
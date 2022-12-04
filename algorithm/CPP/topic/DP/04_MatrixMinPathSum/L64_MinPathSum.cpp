/**
 * @brief   给定一个包含非负整数的mxn网格grid,
 *          请找出一条从左上角到右下角的路径，使得路径上的数字总和为最小
 *          链接:https://leetcode.cn/problems/minimum-path-sum/description/
 *          x   x   x   x
 *          x   x   .   x
 *          x   .   o   x
 *          x   x   x   x
 */
#include <vector>

// 动态规划，时间和空间复杂度为O(MxN)
int minPathSum(std::vector<std::vector<int>>& grid) {
    if (grid.size() == 0 || grid[0].size() == 0) {
        return 0;
    }

    int m = grid.size(), n = grid[0].size();
    std::vector<std::vector<int>> dp(m, std::vector<int>(n));
    dp[0][0] = grid[0][0];
    for (int i = 1; i < m; ++i) {
        dp[i][0] = dp[i-1][0] + grid[i][0];
    }
    for (int i = 1; i < n; ++i) {
        dp[0][i] = dp[0][i-1] + grid[0][i]; 
    }

    for (int i = 1; i < m; ++i) {
        for (int j = 1; j < n; ++j) {
            dp[i][j] = std::min(dp[i-1][j], dp[i][j-1]) + grid[i][j];
        }
    }
    return dp[m-1][n-1];
}

// 动态规划+空间压缩
// 时间复杂度为O(MxN),空间复杂度为O(Min(M,N))

    int minPathSum(vector<vector<int>>& grid) {
        if (grid.size() == 0 || grid[0].size() == 0) {
            return 0;
        }

        int m = grid.size(), n = grid[0].size();
        // 行数<列数为true,否则false
        bool isSmallRow = (m <= n);
        // 行列中较小者,动态规划只需要存储该数量的状态信息即可
        int N = (isSmallRow ? m : n);
        std::vector<int> dp(N);
        dp[0] = grid[0][0];
        for (int i = 1; i < N; ++i) {
            dp[i] = dp[i-1] + (isSmallRow ? grid[i][0] : grid[0][i]);
        }

        int M = (isSmallRow ? n : m); // 行列中较大者
        for (int i = 1; i < M; ++i) {
            dp[0] = dp[0] + (isSmallRow ? grid[0][i] : grid[i][0]);
            for (int j = 1; j < N; ++j) {
                dp[j] = std::min(dp[j-1], dp[j]) + (isSmallRow ? grid[j][i] : grid[i][j]);
            }
        }
        return dp[N-1];
    }
int minPathSum(vector<vector<int>>& grid) {
    if (grid.size() == 0 || grid[0].size() == 0) {
        return 0;
    }

    int m = grid[0].size(), n = grid.size();
    bool isSmallRow = (m <= n); // 行数<列数为true,否则false
    // 行列中较小者,动态规划只需要存储该数量的状态信息即可
    int N = (isSmallRow ? m : n);
    std::vector<int> dp(N);
    dp[0] = grid[0][0];
    for (int i = 1; i < N; ++i) {
        dp[i] = dp[i-1] + (isSmallRow ? grid[0][i] : grid[i][0]);
    }

    int M = (isSmallRow ? n : m); // 行列中较大者
    for (int i = 1; i < M; ++i) {
        dp[0] = dp[0] + (isSmallRow ? grid[i][0] : grid[0][i]);
        for (int j = 1; j < N; ++j) {
            dp[j] = std::min(dp[j-1], dp[j]) + (isSmallRow ? grid[i][j] : grid[j][i]);
        }
    }
    return dp[N-1];
}
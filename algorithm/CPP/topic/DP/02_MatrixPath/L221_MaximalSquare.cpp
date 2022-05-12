/**
 * @brief   221. 最大正方形
 *          在一个由 '0' 和 '1' 组成的二维矩阵内，
 *          找到只包含 '1' 的最大正方形，并返回其面积。
 *          链接：https://leetcode.cn/problems/maximal-square/
 *          
 *          实现思想：
 *              1. 动态规划
 *                  利用两条边将dp[i][0]和dp[j][0]初始化,之后更新dp[i][j]
 *                  if (matrix[i][j] == '1') {
 *                      dp[i][j] = min(dp[i - 1][j - 1], dp[i - 1][j], dp[i][j - 1]) + 1
 *                  } else {
 *                      dp[i][j] = 0;
 *                  }
 *              实现优化：
 *                  增加一层预处理，可以减少if判断
 *                  [
 *                          0   0   0   0   0
 *                      0 ["1","0","1","0","0"],
 *                      0 ["1","0","1","1","1"],
 *                      0 ["1","1","1","1","1"],
 *                      0 ["1","0","0","1","0"]
 *                  ]
 */

int maximalSquare(vector<vector<char>>& matrix) {
    int m = matrix.size(), n = matrix[0].size();
    vector<vector<int>> dp(m, vector<int>(n, 0));
    int maxlen = 0;
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            if (matrix[i][j] == '1') {
                if (i == 0 || j == 0) {
                    dp[i][j] = 1;
                } else {
                    dp[i][j] = min(min(dp[i-1][j], dp[i-1][j-1]), dp[i][j-1])+1; 
                }
                maxlen = max(maxlen, dp[i][j]);
            }
        }
    }
    return maxlen * maxlen;
}

// 增加一层预处理，可以减少if判断
int maximalSquare(vector<vector<char>>& matrix) {
    int m = matrix.size(), n = matrix[0].size();
    vector<vector<int>> dp(m+1, vector<int>(n+1, 0));
    int maxlen = 0;
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            if (matrix[i-1][j-1] == '1') {
                dp[i][j] = min(min(dp[i-1][j], dp[i-1][j-1]), dp[i][j-1])+1; 
                maxlen = max(maxlen, dp[i][j]);
            }
        }
    }
    return maxlen * maxlen;
}
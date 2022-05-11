/**
 * @brief   62.不同路径
 *          
 *          一个机器人位于一个 m x n 网格的左上角 （起始点在下图中标记为 “Start” ）。
 *          机器人每次只能向下或者向右移动一步。机器人试图达到网格的右下角（在下图中标记为 “Finish” ）。
 *          问总共有多少条不同的路径？
 *          链接：https://leetcode.cn/problems/unique-paths
 * 
 *          实现思想：
 *              1. 动态规划
 *                  状态转移方程：f(i,j)=f(i−1,j)+f(i,j−1)
 *                      f[i,j]计算出来之后，f[i-1][j]便无效了
 *                      i从0...m-1, j从0...n-1f[j]表示的就是f[i-1][j]
 *              2. 动态规划+空间压缩
 *                      
 * 
 *              2. 数学组合
 *                  需要移动m+n-2次，其中有 m-1次向下移动，n-1次向右移动。
 *                  因此路径的总数，就等于从m+n-2次移动中选择m-1次向下移动的方案数，即组合数       
 */

// 动态规划,时间和空间复杂度均为O(MN)
int uniquePaths(int m, int n) {
    vector<vector<int>> dp(m, vector<int>(n));
    for (int i = 0; i < m; i++) dp[i][0] = 1;
    for (int i = 0; i < n; i++) dp[0][i] = 1;

    for (int i = 1; i < m; i++) {
        for (int j = 1; j < n; j++) {
            dp[i][j] = dp[i-1][j] + dp[i][j-1];
        }
    }
    return dp[m-1][n-1];
}

// 动态规划+空间压缩
int uniquePaths(int m, int n) {
    vector<int> dp(n, 1);
    for (int i = 1; i < m; i++) {
        for (int j = 1; j < n; j++) {
            dp[j] += dp[j-1];
        }
    }
    return dp[n-1];
}

// 数学组合
int uniquePaths(int m, int n) {
    long long ans = 1;
    if (m > n) swap(m, n);
    for (int x = n, y = 1; y < m; x++, y++) {
        ans = ans * x / y;
    }
    return ans;
}
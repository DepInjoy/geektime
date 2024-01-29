/**
 * 72. 编辑距离
 * https://leetcode.cn/problems/edit-distance/
 * 
 * 状态表示dp[i][j]
 *      将word1[0-i]转换成 word2[0-j]所使用的最少操作数
 * 
 * dp[i-1][j] + 1 : 将i删除，那么问题变换为将
*/

int minDistance(string word1, string word2) {
    const int m = word1.size(), n = word2.size();
    std::vector<std::vector<int> > dp(m+1, std::vector<int>(n+1));
    for (int i = 0; i <= m; ++i) dp[i][0] = i;
    for (int i = 0; i <= n; ++i) dp[0][i] = i;

    for (int i = 1; i <= m; ++i) {
        for(int j = 1; j <= n; ++j) {
            dp[i][j] = std::min(dp[i-1][j], dp[i][j-1]) + 1;
            dp[i][j] = std::min(dp[i][j], dp[i-1][j-1] + (word1[i-1] == word2[j-1] ? 0 : 1));
        }
    }
    return dp[m][n];
}
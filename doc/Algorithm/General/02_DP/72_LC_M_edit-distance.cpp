/**
 * 72. 编辑距离
 * https://leetcode.cn/problems/edit-distance/
 * 
 * 状态表示dp[i][j]
 *  - 将word1[1-i]转换成 word2[1-j]所使用的操作数
 *  - 属性计算：min
 * 
 *  -------------------------------------------------------------------
 *  |   删除word[i]    |     word1[i]插入   |   word[i]替换为word[j]    |
 *  |       sub1       |        sub2       |           sub3            |
 *  -------------------------------------------------------------------
 *   
 *  sub1 : dp[i-1][j] + 1
 *  sub2 : dp[i][j-1] + 1
 *  sub3 : dp[i-1][j-1] + (word1[i] == word2[j] ? 0 : 1)
 * 
 * 状态初始化
 *      dp[i][0] = i, 将a[1~i]删除i次变成b[0]
 *      dp[0][j] = j, 插入i次从a[0]变成b[1~j]
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
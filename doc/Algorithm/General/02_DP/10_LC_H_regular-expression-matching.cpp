/**
 * 10. 正则表达式匹配
 * https://leetcode.cn/problems/regular-expression-matching/description/
 */

bool isMatch(string s, string p) {
    const int m = s.size(), n = p.size();
    std::vector<std::vector<bool>> dp(m+1, std::vector<bool>(n+1));
    dp[0][0] = true;
    auto matched = [&](int i, int j) {
        if (i == 0) return false;
        if (p[j - 1] == '.') return true;
        return s[i - 1] == p[j - 1];
    };

    for (int i = 0; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (p[j - 1] == '*') {
                dp[i][j] = dp[i][j] | dp[i][j-2];
                if (matched(i, j - 1)) dp[i][j] = dp[i][j] | dp[i-1][j];
            } else {
                if (matched(i, j)) {
                    dp[i][j] = dp[i][j] | dp[i - 1][j - 1];
                }
            }
        }
    }
    return dp[m][n];
}
/**
 * @brief   5. 最长回文
 *          给你一个字符串 s，找到 s 中最长的回文子串。
 *
 *          实现思想：
 *              1. 动态规划，时间和空间复杂度都为O(N^2)
 *                  状态初始化：dp[i][i] = true
 *                  状态转移方程：
 *                      dp[i][j] = false           s[i] != s[j]
 *                                 dp[i+1][j-1]    s[i] == s[j]
 *              2. 中心扩展法，时间复杂度为O(N^2), 空间复杂度为O(1)
 *              3. Manacher
 */

// 动态规划
string longestPalindrome(string s) {
    if (s.size() < 2) return s;

    int n = s.size();
    int start = 0, maxLen = 1;
    // dp[i][i]初始化为true
    vector<vector<bool>> dp(n, vector<bool>(n, true));
    // 先枚举长度
    for (int l = 2; l <= n; l++) {
        // 再枚举开始位置
        for (int i = 0; i < n; i++) {
            int j = i + l -1;
            if (j >= n) break;
            if (s[i] == s[j]) {
                dp[i][j] = dp[i+1][j-1];
            } else {
                dp[i][j] = false;
            }

            if (dp[i][j] && j - i + 1 > maxLen) {
                maxLen = j - i + 1;
                start = i;
            }
        }
    }
    return s.substr(start, maxLen);
}

// 中心扩展法
pair<int, int> expandAroundCenter(const string& s, int left, int right) {
    while(left >= 0 && right < s.size() && s[left] == s[right]) {
        --left;
        right++;
    }
    return pair(left+1, right-1);
}

string longestPalindrome(string s) {
    if (s.size() < 2) return s;

    int start = 0, end = 0;
    for (int i = 0; i < s.size(); i++) {
        pair<int, int> p1 = expandAroundCenter(s, i, i);
        pair<int, int> p2 = expandAroundCenter(s, i, i+1);
        if (p1.second - p1.first  > end - start) {
            start = p1.first;
            end = p1.second;
        }
        if (p2.second - p2.first > end - start) {
            start = p2.first;
            end = p2.second;
        }
    }
    return s.substr(start, end-start+1);
}
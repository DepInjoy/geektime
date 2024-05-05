/**
 *  5. 最长回文子串
 *  https://leetcode.cn/problems/longest-palindromic-substring/description/
 * 
 *  1. 动态规划(时间和空间复杂度为O(N^2))
 *  2. 中心扩散法(时间复杂度为O(N^2),空间复杂度为O(1))
 *  3. Manacher算法(TODO)
 */

// 1. 动态规划
string longestPalindrome(string s) {
    const int n = s.size();
    if (n <= 1) return s;

    int maxLen = 1, begin = 0;
    std::vector<std::vector<bool>> dp(n, std::vector<bool>(n));
    for (int i = 0; i < n; ++i) {
        dp[i][i] = true;
    }

    for (int l = 2; l <= n; ++l) {
        for (int i = 0; i < n; ++i) {
            int j = i + l - 1;
            if (j >= n) break;

            if (s[i] != s[j]) {
                dp[i][j] = false;
            } else {
                if (j - i < 3) {
                    dp[i][j] = true;
                } else {
                    dp[i][j] = dp[i+1][j-1];
                }
            }

            if (dp[i][j] && j - i + 1 > maxLen) {
                maxLen = j - i + 1;
                begin = i;
            }
        }
    }
    return s.substr(begin, maxLen);
}

// 2. 中心扩散法
std::pair<int, int> expandAroundCenter(const std::string& s, int l, int r) {
    while(l >= 0 && r < s.size() && s[l] == s[r]) {
        --l, ++r;
    }
    return {l + 1, r -1};
}

string longestPalindrome(string s) {
    int start = 0, end = 0;
    for (int i = 0; i < s.size(); ++i) {
        auto[l1, r1] = expandAroundCenter(s, i, i);
        auto[l2, r2] = expandAroundCenter(s, i, i + 1);
        if (r2 - l2 > end - start) {
            start = l2, end = r2;
        }

        if (r1 - l1 > end - start) {
            start = l1, end = r1;
        }
    }
    return s.substr(start, end - start + 1);
}

// 3. Manacher算法


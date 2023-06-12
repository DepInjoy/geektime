#include <vector>
#include <string>
#include <iostream>

using namespace std;
/**
 * ---------------------------------- 线性DP ---------------------------------
 * 
*/

/**
 *  72. 编辑距离
 *  https://leetcode.cn/problems/edit-distance/description/
 */
int minDistance(string word1, string word2) {
    const int m = word1.size(), n = word2.size();
    std::vector<std::vector<int> > dp(m+1, std::vector<int>(n+1));
    for (int i = 0; i <= m; ++i) dp[i][0] = i;
    for (int i = 0; i <= n; ++i) dp[0][i] = i;

    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            dp[i+1][j+1] = std::min(dp[i][j+1], dp[i+1][j]) + 1;
            dp[i+1][j+1] = std::min(dp[i+1][j+1], dp[i][j] + (word1[i] == word2[j] ? 0 : 1));
        }
    }
    return dp[m][n];
}

/**
 * 53. 最大子数组和
 * https://leetcode.cn/problems/maximum-subarray/description/
*/
int maxSubArray(vector<int>& nums) {
    const int n = nums.size();
    std::vector<int> dp(n+1, 0);
    int ans = INT_MIN;
    for (int i = 0; i < n; ++i) {
        dp[i+1] = std::max(nums[i], dp[i]+nums[i]);
        ans = std::max(ans, dp[i+1]);
    }
    return ans;
}

// 空间压缩
int maxSubArray(vector<int>& nums) {
    const int n = nums.size();
    int pre = 0, cur = 0;
    int ans = INT_MIN;
    for (int i = 0; i < n; ++i) {
        cur = std::max(nums[i], pre+nums[i]);
        ans = std::max(ans, cur);
        pre = cur;
    }
    return ans;
}

// 分治法 todo

/**
 *  5. 最长回文子串
 *  https://leetcode.cn/problems/longest-palindromic-substring/description/
 * 
 *  dp[i][j] = dp[i+1][j-1] and s[i]==s[j]
*/

// 动态规划 时间复杂度O(n^2)，空间复杂度为O(n^2)
string longestPalindrome(string s) {
    const int n = s.size();
    std::vector<std::vector<bool>> dp(n, std::vector<bool>(n, false));
    for (int i = 0; i < n; ++i) dp[i][i] = true;

    int start = 0, maxlen = 1;
    for (int l = 2; l <= n; ++l) {
        for (int i = 0; i < n; ++i) {
            int j = l - 1 + i;
            if (j >= n) break;

            dp[i][j] = (s[i] == s[j]);
            if (s[i] == s[j] && j - i + 1 > 3) {
                dp[i][j] = dp[i+1][j-1];
            }

            if (dp[i][j] && j - i + 1 > maxlen) {
                maxlen = j - i + 1;
                start = i;
            }
        }
    }
    return s.substr(start, maxlen);
}

// 中心扩散法 时间复杂度O(n^2)，空间复杂度为O(1)
int expandAroundCenter(const std::string& s, int l, int r) {
    while (l >= 0 && r < s.size() && s[l] == s[r]) {
        --l, ++r;
    }
    return r - l - 1;
}

string longestPalindrome(string s) {
    const int n = s.size();
    int maxlen = 0, start = 0;
    for (int i = 0; i < n; ++i) {
        int len1 = expandAroundCenter(s, i, i);
        int len2 = expandAroundCenter(s, i, i+1);
        int curmax = std::max(len1, len2);
        if (curmax > maxlen) {
            start = i - ((curmax-1)/2);
            maxlen = curmax;
        }
    }
    return s.substr(start, maxlen);
}

// Manacher, todo

/**
 * 10. 正则表达式匹配(困难)
 * https://leetcode.cn/problems/regular-expression-matching/description/
*/


/**
 *  62. 不同路径(中等)
 *  https://leetcode.cn/problems/unique-paths/description/
*/
int uniquePaths(int m, int n) {
    std::vector<std::vector<int>> dp(m, std::vector<int>(n,0));
    for (int i = 0; i < m; ++i) dp[i][0] = 1;
    for (int i = 0; i < n; ++i) dp[0][i] = 1;

    for (int i = 1; i < m; ++i) {
        for (int j = 1; j < n; ++j) {
            dp[i][j] = dp[i-1][j] + dp[i][j-1];
        }
    }
    return dp[m-1][n-1];
}

// 空间压缩
int uniquePaths(int m, int n) {
    std::vector<int> dp(n, 1);
    for (int i = 1; i < m; ++i) {
        for (int j = 1; j < n; ++j) {
            dp[j] = dp[j-1]+ dp[j];
        }
    }
    return dp[n-1];
}

// 排列组合 C(m+n-2)(m-1) = (m+n-2 ... n) / (m-1)!
int uniquePaths(int m, int n) {
    long long ans = 1;
    for (int i = n, j = 1; j < m; ++i, ++j) {
        ans =  ans * i / j;
    }
    return ans;
}
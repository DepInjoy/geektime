/**
 * 300. 最长递增子序列
 * https://leetcode.cn/problems/longest-increasing-subsequence/
*/

// 动态规划
int lengthOfLIS(vector<int>& nums) {
    const int n = nums.size();
    std::vector<int> dp(n);
    int maxLen = 0;
    for (int i = 0; i < n; ++i) {
        dp[i] = 1;
        for (int j = 0; j < i; ++j) {
            if (nums[i] > nums[j]) {
                dp[i] = std::max(dp[i], dp[j] + 1);
            }
        }
        maxLen = std::max(dp[i], maxLen);
    }
    return maxLen;
}


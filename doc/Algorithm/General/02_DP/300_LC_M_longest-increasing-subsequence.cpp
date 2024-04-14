/**
 * 300. 最长递增子序列
 * https://leetcode.cn/problems/longest-increasing-subsequence/
*/

// 动态规划 时间复杂度O(N^2), 空间复杂度O(N)
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

// 贪心+二分查找 时间复杂度O(NlogN), 空间复杂度O(N)
int lengthOfLIS(vector<int>& nums) {
    const int n = nums.size();
    if (n == 0) return 0;

    int len = 1;
    std::vector<int> d(n+1);
    d[len] = nums[0];
    for (int i = 1; i < n; ++i) {
        if (nums[i] > d[len]) {
            d[++len] = nums[i];
        } else {
            int l = 1, r = len, pos = 0;
            while (l <= r) {
                int mid = (l + r) >> 1;
                if (d[mid] < nums[i]) {
                    pos = mid;
                    l = mid + 1;
                } else {
                    r = mid - 1;
                }
            }
            d[pos + 1] = nums[i];
        }
    }
    return len;
}
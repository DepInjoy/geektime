/**
 * 300. 最长递增子序列
 * https://leetcode.cn/problems/longest-increasing-subsequence/
*/

// 动态规划 时间复杂度O(N^2), 空间复杂度O(N)
int lengthOfLIS(vector<int>& nums) {
    const int n = nums.size();
    std::vector<int> dp(n, 1);
    int ans = 0;
    for (int i = 0; i < n; ++i) {
        for (int j = i; j >= 0; --j) {
            if (nums[j] < nums[i]) {
                dp[i] = std::max(dp[i], dp[j] + 1);
            }
            ans = std::max(ans, dp[i]);
        }
    }
    return ans;
}

// 贪心+二分查找 时间复杂度O(NlogN), 空间复杂度O(N)
int lengthOfLIS(vector<int>& nums) {
    const int n = nums.size();
    std::vector<int> data(n+1);
    int len = 1;
    data[len] = nums[0];
    for (int i = 1; i < n; ++i) {
        if (data[len] < nums[i]) {
            data[++len] = nums[i];
        } else {
            // 在data中查找比nums[i]小的最右的位置
            int l = 1, r = len, pos = 0;
            while (l <= r) {
                int mid = (l + r + 1) >> 1;
                if (data[mid] < nums[i]) {
                    pos = mid;
                    l = mid + 1;
                } else {
                    r = mid - 1;
                }
            }
            data[pos + 1] = nums[i];
        }
    }
    return len;
}
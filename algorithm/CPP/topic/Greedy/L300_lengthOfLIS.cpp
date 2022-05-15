/**
 * @brief   给你一个整数数组 nums ，找到其中最长严格递增子序列的长度。
 * 
 *          子序列 是由数组派生而来的序列，删除（或不删除）数组中的元素而不改变其余元素的顺序。
 *          例如，[3,6,2,7] 是数组 [0,3,1,6,2,2,7] 的子序列。
 *          链接：https://leetcode.cn/problems/longest-increasing-subsequence
 * 
 *          实现思想：
 *              1. 动态规划,时间复杂度为O(N^2)，空间复杂度为O(N)
 *                  dp[i]表示所有以第i个数结尾的上升子序列的最长长度
 *                  状态转移方程：dp[i] = max(dp[i], dp[j]+1) j = 0,1,.....i-1
 *              2. 贪心
 * 
 */

int lengthOfLIS(vector<int>& nums) {
    int n = nums.size();
    int maxlen = 0;
    vector<int> dp(n, 1);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < i; j++) {
            if (nums[j] < nums[i]) {
                dp[i] = max(dp[i], dp[j] + 1);
            }
        }
        maxlen = max(dp[i], maxlen);
    }
    return maxlen;
}

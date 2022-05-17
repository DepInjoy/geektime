/**
 * @brief   给你一个由 不同 整数组成的数组 nums ，和一个目标整数 target
 *          请你从 nums 中找出并返回总和为 target 的元素组合的个数。
 *          题目数据保证答案符合 32 位整数范围。
 *          链接：https://leetcode.cn/problems/combination-sum-iv
 * 
 *          实现思想：
 *              1. 动态规划
 */

// 动态规划
int combinationSum4(vector<int>& nums, int target) {
    vector<int> dp(target+1);
    dp[0] = 1;
    for (int i = 1; i <= target; i++) {
        for (int j = 0; j < nums.size(); j++) {
            // dp[i-nums[j]]+dp[i]可能超出int数据范围
            if (nums[j] <= i && dp[i-nums[j]] < INT_MAX - dp[i]) {
                dp[i] += dp[i-nums[j]];
            }
        }
    }
    return dp[target];
}
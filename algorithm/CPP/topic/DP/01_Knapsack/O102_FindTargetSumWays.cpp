/**
 * @brief   给定一个正整数数组 nums 和一个整数 target 。
 *          向数组中的每个整数前添加 '+' 或 '-' ，然后串联起所有整数，可以构造一个 表达式 ：
 *          例如，nums = [2, 1] ，可以在 2 之前添加 '+' ，在 1 之前添加 '-' ，然后串联起来得到表达式 "+2-1" 。
 *          返回可以通过上述方法构造的、运算结果等于 target 的不同 表达式 的数目。
 *          
 *          链接：https://leetcode.cn/problems/YaVDxD
 * 
 *          实现思想：
 *              1. 动态规划,01背包
 * 
 */

int findTargetSumWays(vector<int>& nums, int target) {
    int sum = 0;
    for (int i = 0; i < nums.size(); i++) sum += nums[i];
    target = (sum - target);
    if (target < 0 || target & 1) return 0;

    target = target >> 1;
    vector<int> dp(target+1);
    dp[0] = 1;
    for (int i = 0; i < nums.size(); i++) {
        for (int j = target; j >= nums[i]; j--) {
            dp[j] += dp[j-nums[i]];
        }
    }
    return dp[target];
}
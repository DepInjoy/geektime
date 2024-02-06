/**
 * 377. 组合总和 Ⅳ
 * https://leetcode.cn/problems/combination-sum-iv/description/
*/

int combinationSum4(vector<int>& nums, int target) {
    std::vector<int> dp(target+1);
    dp[0] = 1;
    for (int i = 0; i <= target; ++i) {
        for (int num : nums) {
            if (num <= i && dp[i-num] < INT_MAX - dp[i]) dp[i] += dp[i-num];
        }
    }
    return dp[target];
}
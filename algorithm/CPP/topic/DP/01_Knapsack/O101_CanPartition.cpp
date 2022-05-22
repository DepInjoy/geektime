/**
 * @brief   分割等和子集
 *              给定一个非空的正整数数组，请判断能否将这些数字分成和相等的两部分。
 *              例如，如果输入数组为[3，4，1]，将这些数字分成[3，1]和[4]两部分，输出true
 *              如果输入数组为[1，2，3，5]，则不能将这些数字分成和相等的两部分，因此输出false
 * 
 *              实现思想：
 *                  1. 动态规划，01背包问题
 */

bool canPartition(vector<int>& nums) {
    int sum = 0;
    for (int i = 0; i < nums.size(); i++) sum += nums[i];
    if (sum & 1) return false;

    int target = sum >> 1;
    vector<bool> dp(target+1);
    dp[0] = true;
    for (int i = 0; i < nums.size(); i++) {
        for (int j = target; j >= nums[i]; j--) {
            dp[j] = dp[j] | dp[j-nums[i]];
        }
    }
    return dp[target];
}
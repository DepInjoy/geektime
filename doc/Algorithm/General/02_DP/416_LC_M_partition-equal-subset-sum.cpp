/**
 * 416. 分割等和子集
 * https://leetcode.cn/problems/partition-equal-subset-sum/description/
 * 
 * dp[i][j] 从[0,i]中选取某些元素和恰好为j
 * |------------------------------------------------|
 * |    sub1(不选i)        |       sub2(选取i)       |
 * |------------------------------------------------|
 * 
 * sub1:  dp[i-1][j-num[i]]
 * sub2： dp[i-1][j]
 * 
 * dp[i][0] = true  不选取任何元素恰好是0
*/

bool canPartition(vector<int>& nums) {
    int target = 0, n = nums.size(), maxVal = 0;
    for (int& num : nums) {
        target += num;
        maxVal = std::max(maxVal, num);
    }
    if (target % 2 || n <2 || maxVal > target >> 1) return false;
    
    target = target >> 1;
    std::vector<int> dp(target+1);
    dp[0] = true;
    for (int i = 0; i < nums.size(); ++i) {
        for (int j = target; j >= nums[i]; --j) {
            dp[j] = dp[j] | dp[j - nums[i]];
        }
    }
    return dp[target];
}
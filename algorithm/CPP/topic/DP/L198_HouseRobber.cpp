/**
 * @brief   你是一个专业的小偷，计划偷窃沿街的房屋。每间房内都藏有一定的现金，
 *          影响你偷窃的唯一制约因素就是相邻的房屋装有相互连通的防盗系统，
 *          如果两间相邻的房屋在同一晚上被小偷闯入，系统会自动报警。
 *          给定一个代表每个房屋存放金额的非负整数数组，计算你不触动警报装置的情况下
 *          一夜之内能够偷窃到的最高金额。
 *          链接：https://leetcode-cn.com/problems/house-robber
 * 
 */
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

int rob(vector<int>& nums) {
    int N = nums.size();
    vector<int> dp = vector<int>(N+2, 0);
    for (int i = N-1; i <= 0; i++) {
        dp[i] = max(dp[i+2] + nums[i], dp[i+1]);
    }
    return dp[0];
}
/**
 * @brief   你是一个专业的小偷，计划偷窃沿街的房屋。每间房内都藏有一定的现金，
 *          影响你偷窃的唯一制约因素就是相邻的房屋装有相互连通的防盗系统，
 *          如果两间相邻的房屋在同一晚上被小偷闯入，系统会自动报警。
 *          给定一个代表每个房屋存放金额的非负整数数组，计算你不触动警报装置的情况下
 *          一夜之内能够偷窃到的最高金额。
 *          链接：https://leetcode-cn.com/problems/house-robber
 *      
 *          实现思想：
 *              1. 动态规划
 *                  状态转移方程：dp[i]=max(dp[i−2]+nums[i],dp[i−1])
 *              2. 动态规划+滚动数组
 */
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;
// 动态规划，数据填充，可以按照从前向后和从后向前两种方式实现
int rob(vector<int>& nums) {
    int N = nums.size();
    vector<int> dp = vector<int>(N+2, 0);
    for (int i = N-1; i >= 0; i--) {
        dp[i] = max(dp[i+2] + nums[i], dp[i+1]);
    }
    return dp[0];
}

int rob(vector<int>& nums) {
    int N = nums.size();
    vector<int> dp(N+2);
    dp[0] = 0, dp[1] = 0;
    for (int i = 2; i < nums.size()+2; i++) {
        dp[i] = max(dp[i-2] + nums[i-2], dp[i-1]);
    }
    return dp[N+1];
}

// 动态规划+滚动数组
int rob(vector<int>& nums) {
    int prepre = 0, pre = 0, cur = 0;
    for (int i = 0; i < nums.size(); i++) {
        cur = max(prepre + nums[i], pre);
        prepre = pre;
        pre = cur;
    }
    return cur;
}
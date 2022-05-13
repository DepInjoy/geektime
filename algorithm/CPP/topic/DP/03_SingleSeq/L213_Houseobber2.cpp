/**
 * @brief   你是一个专业的小偷，计划偷窃沿街的房屋，每间房内都藏有一定的现金
 *          这个地方所有的房屋都 围成一圈 ，这意味着第一个房屋和最后一个房屋是紧挨着的
 *          同时，相邻的房屋装有相互连通的防盗系统，如果两间相邻的房屋在同一晚上被小偷闯入
 *          系统会自动报警.给定一个代表每个房屋存放金额的非负整数数组
 *          计算你在不触动警报装置的情况下 ，今晚能够偷窃到的最高金额。
 *          链接：https://leetcode-cn.com/problems/house-robber-ii
 * 
 *          实现思想：
 *              1. 动态规划
 *                  状态转移方程： dp[i]=max(dp[i−2]+nums[i],dp[i−1])
 *                      得到计算得到dp的下标范围为[start, end]内可以偷窃到的最高总金额
 *                  最终结果：[0,n-2]和[1,n-1]可以偷窃到的最高总金额的最大值
 */
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;
int rob(vector<int>& nums, int start, int end) {
    int prepre = 0, pre = 0, cur = 0;
    for (int i = start; i <= end; i++) {
        cur = max(prepre + nums[i], pre);
        prepre = pre;
        pre = cur;
    }
    return cur;
}

int rob(vector<int>& nums) {
    int n = nums.size();
    if (n == 1) {
        return nums[0];
    } else if (n == 2) {
        return max(nums[0], nums[1]);
    } else {
        return max(rob(nums, 0, n-2), rob(nums, 1, n-1));
    }
}
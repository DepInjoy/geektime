/**
 * 213. 打家劫舍 II
 *  https://leetcode-cn.com/problems/house-robber-ii/
 * 
 *  剑指 Offer II 090. 环形房屋偷盗
 *  https://leetcode.cn/problems/PzWKhm/description/
 * 
 *  一个专业的小偷，计划偷窃一个环形街道上沿街的房屋，每间房内都藏有一定的现金。
 *  这个地方所有的房屋都 围成一圈 ，这意味着第一个房屋和最后一个房屋是紧挨着的。
 *  同时，相邻的房屋装有相互连通的防盗系统，如果两间相邻的房屋在同一晚上被小偷闯入，系统会自动报警。
 *  给定一个代表每个房屋存放金额的非负整数数组 nums ，请计算 在不触动警报装置的情况下 ，今晚能够偷窃到的最高金额。
*/

#include <vector>
using namespace std;

int rob(vector<int>& nums) {
    auto rob = [](const vector<int>& nums, int start, int end) -> int {
        int prepre = 0, pre = 0;
        for (int i = start; i < end; ++i) {
            int cur =  std::max(prepre + nums[i], pre);
            prepre = pre;
            pre = cur;
        }
        return pre;
    };

    const int n = nums.size();
    if (n == 1) return nums[0];
    return std::max(rob(nums, 0, n-1), rob(nums, 1, n));
}

int rob(vector<int>& nums) {
    const int n = nums.size();
    // 偷第一个，不偷最后一个
    int prepre = 0, pre = nums[0];
    for (int i = 1; i < n - 1; ++i) {
        int cur = std::max(prepre + nums[i], pre);
        prepre = pre;
        pre = cur;
    }

    // 不偷第一个，偷最后一个
    int prepre2 = 0, pre2 = 0;
    for (int i = 1; i < n; ++i) {
        int cur = std::max(prepre2 + nums[i], pre2);
        prepre2 = pre2;
        pre2 = cur;
    }
    return std::max(pre, pre2);
}
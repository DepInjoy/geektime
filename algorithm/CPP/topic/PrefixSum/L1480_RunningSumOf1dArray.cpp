/**
 * @brief:  给你一个数组 nums 。数组「动态和」的计算公式为：
 *          runningSum[i] = sum(nums[0]…nums[i])
 *          请返回 nums 的动态和。
 *          链接：https://leetcode.cn/problems/running-sum-of-1d-array
*/

// 原地修改
vector<int> runningSum(vector<int>& nums) {
    for (int i = 1; i < nums.size(); i++) {
        nums[i] += nums[i-1];
    }
    return nums;
}
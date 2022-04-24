/**
 * @brief   给你一个长度为 n 的整数数组 nums 和 一个目标值 target
 *          请你从 nums 中选出三个整数，使它们的和与 target 最接近
 *          返回这三个数的和。假定每组输入只存在恰好一个解。
 *          链接：https://leetcode-cn.com/problems/3sum-closest
 * 
 */
#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

int threeSumClosest(vector<int>& nums, int target) {
    int N = nums.size();
    sort(nums.begin(), nums.end());
    int ans = nums[0] + nums[1] + nums[2];
    for (int i = 0; i < N; i++) {
        int j = i + 1, k = N -1;
        while (j < k) {
            int sum = nums[i] + nums[j] + nums[k];
            if (abs(sum - target) < abs(ans - target)) {
                ans = sum;
            }
            if (sum > target) {
                k--;
            } else if (sum == target) {
                return ans;
            } else {
                j++;
            }
        }
    }
    return ans;
}

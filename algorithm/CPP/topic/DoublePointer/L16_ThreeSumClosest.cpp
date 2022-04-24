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
    int delta = INT_MAX;
    int ai = 0, aj = 0; ak = 0;
    sort(nums.begin(), nums.end());
    for (int i = 0; i < N; i++) {
        for (int j = i + 1, k = N -1; j < k; j++) {
            while (k > j && abs(arr[i] + arr[j] + arr[k] - target)
                    >= abs(arr[i] + arr[j - 1] + arr[k] - target)) j--;
            if (delta > abs(arr[i] + arr[j] + arr[k] - target)) {
                ai = i, aj = j, ak = k;
            }
    }
    return nums[ai] + nums[aj] + nums[ak];
}

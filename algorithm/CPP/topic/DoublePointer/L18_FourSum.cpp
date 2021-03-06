/**
 * @brief   给你一个由 n 个整数组成的数组nums，和一个目标值 target
 *          请你找出并返回满足下述全部条件且不重复的四元组
 *          [nums[a],nums[b],nums[c],nums[d]]
 *          （若两个四元组元素一一对应，则认为两个四元组重复）：
 *          0 <= a, b, c, d < n, a、b、c 和 d 互不相同
 *          nums[a] + nums[b] + nums[c] + nums[d] == target
 *          链接：https://leetcode-cn.com/problems/4sum
 * 
 */
#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

vector<vector<int>> fourSum(vector<int>& nums, int target) {
    int N = nums.size();
    sort(nums.begin(), nums.end());

    vector<vector<int>> ans;
    for (int a = 0; a < N; a++) {
        int rest = target - nums[a];
        if (a == 0 || nums[a] != nums[a-1]) {
            for (int b = a + 1; b < N; b++) {
                int rest2 = rest - nums[b];
                if (b == a +1 || nums[b] != nums[b-1]){
                    for (int c = b + 1, d = N - 1; c < d; c++) {
                        if (c != b + 1 && nums[c] == nums[c-1]) continue;
                        while (d > c && nums[c] + nums[d] > rest2) d--;
                        if (d > c && nums[c] + nums[d] == rest2) {
                            ans.push_back({nums[a], nums[b], nums[c], nums[d]});
                        }
                    }
                }
            }
        }
    }
    return ans;
}
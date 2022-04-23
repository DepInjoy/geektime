/**
 * @brief   给你一个包含 n 个整数的数组 nums，判断 nums 中是否存在三个元素 a，b，c
 *          使得 a + b + c = 0 ？请你找出所有和为 0 且不重复的三元组
 *          
 *          链接：https://leetcode-cn.com/problems/3sum
 * 
 * 实现思想：
 *      排序+双指针
 * 
 *      不重复的本质是什么？我们保持三重循环的大框架不变，只需要保证:
 *          1. 第二重循环枚举到的元素不小于当前第一重循环枚举到的元素
 *          2. 第三重循环枚举到的元素不小于当前第二重循环枚举到的元素
 *      
 * 复杂度分析：
 *          时间复杂度：O(N^2), 其中N是数组nums的长度
 *          空间复杂度：O(logN), 我们忽略存储答案的空间，额外的排序的空间复杂度为 O(logN)
 *          需要一个额外的数组存储nums的副本并进行排序，空间复杂度为O(N)
 */
#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

vector<vector<int>> threeSum(vector<int>& nums) {
    vector<vector<int>> ans;
    sort(nums.begin(), nums.end());
    int N = nums.size();
    for (int i = 0; i < N; i++) {
        int target = -nums[i];
        if (i > 0 && nums[i] == nums[i-1]) continue;
        for (int j = i+1, k = N-1;j < k; j++) {
            if (j > i + 1 && nums[j] == nums[j-1]) continue;
            while(k > j && nums[j] + nums[k] > target) k--;
            if (j != k && nums[j] + nums[k] == target) {
                ans.push_back(std::vector<int>{nums[i], nums[j], nums[k]});
            }
        }
    }
    return ans;
}

int main() {
    return 0;
}
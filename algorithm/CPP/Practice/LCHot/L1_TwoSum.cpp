/**
 * @brief   1. 两数之和
 *          给定一个整数数组nums和一个整数目标值target
 *          请你在该数组中找出和为目标值target的两个整数
 *          并返回它们的数组下标。
 *          你可以假设每种输入只会对应一个答案。但是，数组中同一个元素在答案里不能重复出现。
 *          可以按任意顺序返回答案。
 *          链接：https://leetcode.cn/problems/two-sum
 * 
 *          实现思想：
 *              1. Hash表，时间复杂度O(N), 空间复杂度O(N)
 * 
 */

vector<int> twoSum(vector<int>& nums, int target) {
    unordered_map<int, int> valmap;
    for (int i = 0; i < nums.size(); i++) {
        auto rest = valmap.find(target-nums[i]);
        if (rest != valmap.end()) {
            return vector<int>{i, rest->second};
        }
        valmap[nums[i]] = i;
    }
    return vector<int>{};
}
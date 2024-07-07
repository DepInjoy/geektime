/**
 * 283. 移动零
 * https://leetcode.cn/problems/move-zeroes
 * 
 * 实现方式：
 *  1. 双指针
*/

void moveZeroes(vector<int>& nums) {
    int l = 0, r = 0;
    while (r < nums.size()) {
        if (nums[r]) {
            std::swap(nums[l++], nums[r]);
        }
        ++r;
    }
}
[283. 移动零](https://leetcode.cn/problems/move-zeroes)
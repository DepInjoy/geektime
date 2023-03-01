#include <vector>
#include <algorithm>

using namespace std;

/**
 * @brief   面试题61:扑克牌中的顺子
 *              从若干副扑克牌中随机抽 5 张牌，判断是不是一个顺子，即这5张牌是不是连续的。
 *              2～10为数字本身，A为1，J为11，Q为12，K为13，而大、小王为0，可以看成任意数字
 *              A不能视为 14。
 * 
 *          https://leetcode.cn/problems/bu-ke-pai-zhong-de-shun-zi-lcof/description
 */
bool isStraight(vector<int>& nums) {
    int zero_num = 0, i = 0;
    std::sort(nums.begin(), nums.end());
    for (; i < nums.size() && nums[i] == 0; ++i) {
        ++zero_num;
    }
    int pre = nums[i++];
    for (; i < nums.size(); ++i) {
        zero_num -= nums[i] - pre - 1;
        if (zero_num < 0 || nums[i] == pre) {
            return false;
        }
        pre = nums[i];
    }
    return true;
}
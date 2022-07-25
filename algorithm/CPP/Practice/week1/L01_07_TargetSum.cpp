/**
 * @brief       给定一个数组arr，你可以在每个数字之前决定+或者-
 *              但是必须所有数字都参与,再给定一个数target
 *              请问最后算出target的方法数是多少？
 *              
 *              https://leetcode-cn.com/problems/target-sum/
 * 
 */

#include <vector>
#include <iostream>

using namespace std;

// 暴力递归实现
int process(const std::vector<int>& nums, int index, int rest) {
    if (index == nums.size()) {
        return rest == 0 ? 1 : 0;
    }

    return process(nums, index + 1, rest - nums[index]) +
            process(nums, index + 1, rest + nums[index]);
}

int findTargetSumWays(const vector<int>& nums, int target) {
    return process(nums, 0, target);
}

/*
    输入：nums = [1,1,1,1,1], target = 3
    输出：5

    输入：nums = [1], target = 1
    输出：1
*/
int main(int argc, char* argv[]) {
    std::vector<int> nums { 1, 1, 1, 1, 1 };
    int target = 3;
    std::cout << findTargetSumWays(nums, target) << std::endl;
    return 0;
}
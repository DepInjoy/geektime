/**
 * 78. 子集
 * https://leetcode.cn/problems/subsets/description/
 * 
 * 实现方式
 *  1. 回溯
*/

// 回溯
void backtrace(std::vector<std::vector<int>>& data, std::vector<int>& output,
        std::vector<int>& nums, int pos, int len) {
    if (pos == len) {
        data.emplace_back(output);
        return;
    }

    backtrace(data, output, nums, pos+1, len);

    output.emplace_back(nums[pos]);
    backtrace(data, output, nums, pos+1, len);
    output.pop_back();
}

vector<vector<int>> subsets(vector<int>& nums) {
    std::vector<std::vector<int>> data;
    std::vector<int> output;
    backtrace(data, output, nums, 0, nums.size());
    return data;
}


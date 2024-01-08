/**
 * 1. 两数之和
 * https://leetcode.cn/problems/two-sum/description/
*/

// Hash表, 时间和空间复杂度O(N)
vector<int> twoSum(vector<int>& nums, int target) {
    const int n = nums.size();
    std::unordered_map<int, int> umap(n);
    for (int i = 0; i < n; ++i) {
        auto item = umap.find(target - nums[i]);
        if (item != umap.end()) {
            return {item->second, i};
        }
        umap[nums[i]] = i;
    }
    return {};
}

// 暴力求解 时间复杂度O(N^2), 空间复杂度O(N)
vector<int> twoSum(vector<int>& nums, int target) {
    const int n = nums.size();
    for (int  i = 0; i < n; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (nums[i] + nums[j] == target) {
                return {i, j};
            }
        }
    }
    return std::vector<int>{};
}
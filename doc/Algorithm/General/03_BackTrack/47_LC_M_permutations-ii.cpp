/**
 * 47. 全排列ii
 * https://leetcode.cn/problems/permutations-ii/description/
 * https://leetcode.cn/problems/7p8L0Z/description/
 * 
 * 题目描述：
 *   给定一个可包含重复数字的序列 nums ，按任意顺序 返回所有不重复的全排列
*/

void dfs(const std::vector<int>& nums, int index, std::vector<bool>& visited,
        std::vector<int>& permute, std::vector<std::vector<int>>& ans) {
    if (index == nums.size()) {
        ans.emplace_back(permute);
        return;
    }

    for (int i = 0; i < nums.size(); ++i) {
        // !visited[i-1]同层剪枝效率更高
        // visited[i-1]非同层剪枝
        if (visited[i] || (i > 0 && nums[i] == nums[i-1] && !visited[i-1])) {
            continue;
        }

        permute.emplace_back(nums[i]);
        visited[i] = true;
        dfs(nums, index+1, visited, permute, ans);
        visited[i] = false;
        permute.pop_back();
    }
}

vector<vector<int>> permuteUnique(vector<int>& nums) {
    std::sort(nums.begin(), nums.end());
    const int n = nums.size();
    std::vector<bool> visited(n);
    std::vector<int> permute;
    permute.reserve(n);
    std::vector<std::vector<int>> ans;
    dfs(nums, 0, visited, permute, ans);
    return ans;
}
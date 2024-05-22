/**
 * 46. 全排列
 * https://leetcode.cn/problems/permutations/
*/

// 回溯
void backtrace(std::vector<std::vector<int>>& ans,
    std::vector<int>& output, int pos, int len) {
    // 所有数据都处理完了，产生一个输出
    if (pos == len) {
        ans.push_back(output);
        return;
    }

    for (int i = pos; i < len; ++i) {
        // 动态维护数组
        std::swap(output[pos], output[i]);
        // 递归处理下一个数
        backtrace(ans, output, pos + 1, len);
        // 恢复现场
        std::swap(output[pos], output[i]);
    }
}

vector<vector<int>> permute(vector<int>& nums) {
    std::vector<std::vector<int>> ans;
    backtrace(ans, nums, 0, nums.size());
    return ans;
}

// 采用visited实现回溯
void backtrace(std::vector<std::vector<int>>& ans,
    std::vector<bool>& visited, std::vector<int>& path,
    std::vector<int>& nums, int depth, int len) {
    // 所有数据都处理完了，产生一个输出
    if (depth == len) {
        ans.push_back(path);
        return;
    }

    for (int i = 0; i < len; ++i) {
        if (!visited[i]) {
            visited[i] = true;
            path.push_back(nums[i]);
            backtrace(ans, visited, path, nums, depth+1, len);
            visited[i] = false;
            path.pop_back();
        }
    }
}

vector<vector<int>> permute(vector<int>& nums) {
    std::vector<std::vector<int>> ans;
    const int n = nums.size();
    std::vector<bool> visited(n);
    std::vector<int> path;
    path.reserve(n);
    backtrace(ans, visited, path, nums, 0, nums.size());
    return ans;
}
/**
 * 128. 最长连续序列
 * https://leetcode.cn/problems/longest-consecutive-sequence
*/

int longestConsecutive(vector<int>& nums) {
    const int n = nums.size();
    std::unordered_set<int> usets;
    for (int num : nums) usets.insert(num);

    int max_len = 0;
    for (int num : nums) {
        if (!usets.count(num - 1)) {
            int cur_num = num, cur_len = 1;
            while (usets.count(cur_num + 1)) {
                ++cur_len, ++cur_num;
            }
            max_len = std::max(max_len, cur_len);
        }
    }
    return max_len;
}

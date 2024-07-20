/**
 * 128. 最长连续序列
 * https://leetcode.cn/problems/longest-consecutive-sequence
*/

int longestConsecutive(vector<int>& nums) {
    std::unordered_set<int> uset;
    for (int num : nums) uset.insert(num);

    int mlen = 0;
    for (int num : uset) {
        if (!uset.count(num - 1)) {
            int cur_len = 1, cur = num;
            while (uset.count(++cur)) ++cur_len;
            mlen = std::max(mlen, cur_len); 
        }
    }
    return mlen;
}
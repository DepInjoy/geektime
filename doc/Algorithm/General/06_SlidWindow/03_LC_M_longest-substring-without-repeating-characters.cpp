/**
 *  3. 无重复字符的最长子串
 *  https://leetcode.cn/problems/longest-substring-without-repeating-characters/description/
*/

// 滑动窗口
int lengthOfLongestSubstring(string s) {
    std::unordered_set<char> uset;
    const int n = s.size();
    int max_len= 0, j = -1;
    for (int i = 0; i < n; ++i) {
        if (i) uset.erase(s[i-1]);
        while(j + 1 < n && uset.count(s[j+1]) == 0) {
            uset.insert(s[++j]);
        }
        max_len = std::max(max_len, j - i + 1);
    }
    return max_len;
}


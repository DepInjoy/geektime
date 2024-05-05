/**
 *  3. 无重复字符的最长子串
 *  https://leetcode.cn/problems/longest-substring-without-repeating-characters/description/
 * 
 *  实现方式：
 *    1. 滑动窗口, 时间复杂度O(N), 空间复杂度O(字符集大小)
 */

// 滑动窗口
int lengthOfLongestSubstring(string s) {
    const int n = s.size();
    int maxLen = 0, j = -1;
    std::unordered_set<char> uset(n);
    for (int i = 0; i < n; ++i) {
        if (i != 0) uset.erase(s[i - 1]);
        while (j + 1 < n && !uset.count(s[j + 1])) {
            uset.insert(s[j+1]);
            j++;
        }
        maxLen =  std::max(maxLen, j - i + 1);
    }
    return maxLen;
}


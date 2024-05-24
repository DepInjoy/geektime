/**
 * 438. 找到字符串中所有字母异位词
 * https://leetcode.cn/problems/find-all-anagrams-in-a-string/description/
 * 
 * 实现方式：
 *  1. 滑动窗口
*/

vector<int> findAnagrams(string s, string p) {
    const int sl = s.size(), pl = p.size();
    std::vector<int> ans;
    if (sl >= pl) {
        std::vector<int> cnt(26);
        for (int i = 0; i < pl; ++i) {
            ++cnt[s[i] - 'a'];
            --cnt[p[i] - 'a'];
        }

        int diff = 0;
        for (int i = 0; i < 26; ++i) {
            diff += (cnt[i] ? 1 : 0);
        }
        if (diff == 0) ans.emplace_back(0);

        for (int i = 0; i < sl - pl; ++i) {
            if (cnt[s[i] - 'a'] == 1) {
                --diff;
            } else if (cnt[s[i] - 'a'] == 0) {
                ++diff;
            }
            --cnt[s[i] - 'a'];

            if (cnt[s[i+pl] - 'a'] == -1) {
                --diff;
            } else if (cnt[s[i+pl] - 'a'] == 0) {
                ++diff;
            }
            ++cnt[s[i+pl] - 'a'];
            if (diff == 0) ans.emplace_back(i+1);
        }
    }
    return ans;
}

vector<int> findAnagrams(string s, string p) {
    const int sl = s.size(), pl = p.size();
    std::vector<int> ans;
    if (sl >= pl) {
        std::vector<int> scnt(26), pcnt(26);
        for (int i = 0; i < pl; ++i) {
            ++scnt[s[i] - 'a'];
            ++pcnt[p[i] - 'a'];
        }
        if (scnt == pcnt) ans.emplace_back(0);

        for (int i = 0; i < sl - pl; ++i) {
            --scnt[s[i] - 'a'];
            ++scnt[s[i+pl] - 'a'];
            if (scnt == pcnt) ans.emplace_back(i+1);
        }
    }
    return ans;
}
/**
 * 344. 反转字符串
 * https://leetcode.cn/problems/reverse-string/description/
*/

void reverseString(vector<char>& s) {
    for (int l = 0, r = s.size() - 1; l < r; ++l, --r) {
        std::swap(s[l], s[r]);
    }
}
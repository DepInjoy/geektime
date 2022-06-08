/**
 * @brief   面试题 01.02. 判定是否互为字符重排
 *          链接：https://leetcode.cn/problems/check-permutation-lcci/
 *          
 *          实现思想：
 *              1. 两个字符串排序后相等
 *              2. 两个字符串中字符出现的种类和次数均相等
 */

// 可以通过，题目没有明确只有小写字母，效率最高
bool CheckPermutation(string s1, string s2) {
    if (s1.size() != s2.size()) return false;

    std::vector<int> v(26, 0); // maybe 256
    for (char c : s1) ++v[c - 'a'];

    for (char c : s2) {
        if (--v[c - 'a'] < 0) return false; 
    }
    return true;
}

// 个人认为相对于vector结构，普适性更强
bool CheckPermutation(string s1, string s2) {
    if (s1.size() != s2.size()) return false;

    std::unordered_map<char, int> m1;
    for (char c : s1) {
        m1[c] = (m1.find(c) == m1.end()) ? 1 : m1[c] + 1;
    }

    for (char c : s2) {
        if (m1.find(c) == m1.end() || --m1[c] < 0) return false;
    }
    return true;
}

bool CheckPermutation(string s1, string s2) {
    if (s1.size() != s2.size()) return false;

    std::sort(s1.begin(), s1.end());
    std::sort(s2.begin(), s2.end());
    return s1.compare(s2) == 0;
}
/**
 * @brief   面试题 01.09. 字符串轮转
 *          字符串轮转。给定两个字符串s1和s2，请编写代码检查s2是否为s1旋转而成
 *          （比如，waterbottle是erbottlewat旋转后的字符串）。
 *          链接：https://leetcode.cn/problems/string-rotation-lcci/
 */

// 模拟翻转
bool isFlipedString(string s1, string s2) {
    if (s1.size() != s2.size()) return false;
    if (s1.size() == 0) return true;

    int N = s1.size();
    for (int i = 0; i < s1.size(); ++i) {
        bool flag = false;
        for (int j = 0; j < s2.size();++j) {
            if (s1[(i+j)%N] != s2[j]) {
                flag = true;
                break;
            }
        }
        if (!flag) return true;
    }
    return false;       
}

// 搜索子串，时间复杂度O(N),空间复杂度O(N)
bool isFlipedString(string s1, string s2) {
    return s1.size() == s2.size() && (s2 + s2).find(s1) != string::npos;
}
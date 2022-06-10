/**
 * @brief   01.04. 回文排列
 *          给定一个字符串，编写一个函数判定其是否为某个回文串的排列之一。
 *          回文串是指正反两个方向都一样的单词或短语。排列是指字母的重新排列。
 *          回文串不一定是字典当中的单词。
 *          链接：https://leetcode.cn/problems/palindrome-permutation-lcci
 * 
 */

// 位向量减少空间占用
bool canPermutePalindrome(string s) {
    bitset<128> bset;
    for (char c : s) {
        bset.flip(c);
    }
    return bset.count() <= 1;
}

// 使用Hash表,空间复杂度为O(N)
bool canPermutePalindrome(string s) {
    std::unordered_set<char> uset;
    for (char c : s) {
        if (uset.find(c) != uset.end()) {
            uset.erase(c);
        } else {
            uset.insert(c);
        }
    }
    return uset.size() <= 1;
}
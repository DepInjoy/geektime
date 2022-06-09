/**
 * @brief   判断字符串是否唯一
 *          链接：https://leetcode.cn/problems/is-unique-lcci/
 * 
 *          实现思想：
 *              1. 位运算
 *              2. 利用HashSet
 */

bool isUnique(string astr) {
    int state = 0;
    for (char c : astr) {
        int pos = c - 'a';
        if (state & (1 << pos)) return false;
        state |= (1 << pos);
    }
    return true;
}

bool isUnique(string astr) {
    std::unordered_set<char> set;
    for (char c : astr) {
        if (set.find(c) != set.end()) return false;
        set.insert(c);
    }
    return true;
}
    

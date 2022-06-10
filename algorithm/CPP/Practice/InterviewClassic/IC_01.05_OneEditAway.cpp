/**
 * @brief   01.05. 一次编辑
 *          字符串有三种编辑操作:插入一个英文字符、删除一个英文字符或者替换一个英文字符
 *          给定两个字符串，编写一个函数判定它们是否只需要一次(或者零次)编辑。
 *          链接：https://leetcode.cn/problems/one-away-lcci/
 * 
 */

bool oneEditAway(string first, string second) {
    int diff = first.size()-second.size();
    if (diff > 1 || diff < -1) return false;

    bool flag = false;
    int i = 0, j = 0;
    while (i < first.size() && j < second.size()) {
        if (first[i] == second[j]) {
            i++, j++;
        } else {
            if (flag) return false;
            flag = true;
            (diff > 0) ? i++ : ((diff < 0) ? j++ : (i++, j++));
        }
        if (i - j > 1 || j - i > 1) return false;
    }
    return true;
}
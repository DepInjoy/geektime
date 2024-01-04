/**
 * 1047. 删除字符串中的所有相邻重复项
 * https://leetcode.cn/problems/remove-all-adjacent-duplicates-in-string/description
*/

// 利用std::string提供的类似入栈和出栈的接口
string removeDuplicates(string s) {
    std::string stk;
    for (char ch : s) {
        if (!stk.empty() && stk.back() == ch) {
            stk.pop_back();
        } else {
            stk.push_back(ch);
        }
    }
    return stk;
}
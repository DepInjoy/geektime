/**
 * 20. 有效的括号
 * https://leetcode.cn/problems/valid-parentheses/
*/

bool isValid(string s) {
    if (s.size() % 2) return false;

    std::unordered_map<char, char> matched_parents = {
        {')', '('},
        {']', '['},
        {'}', '{'}
    };
    std::stack<char> stk;
    for (char ch : s) {
        if (matched_parents.count(ch)) {
            if (stk.empty() || stk.top() != matched_parents[ch]) {
                return false;
            }
            stk.pop();
        } else {
            stk.push(ch);
        }
    }
    return stk.empty();
}
/**
 * @brief   20. 有效的括号
 *           链接：https://leetcode.cn/problems/valid-parentheses/
 */

bool isValid(string s) {
    if (s.size() % 2 != 0) return false;

    std::unordered_map<char, char> pairs = {
        {')', '('},
        {']', '['},
        {'}', '{'} 
    };
    std::stack<char> stack;
    for (char ch : s) {
        if (pairs.count(ch)) {
            if (stack.empty() || stack.top() != pairs[ch]) {
                return false;
            }
            stack.pop();
        } else {
            stack.push(ch);
        }
    }
    return stack.empty();
}
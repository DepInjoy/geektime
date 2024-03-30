/**
 *  125. 验证回文
 *  https://leetcode.cn/problems/valid-palindrome/description
*/

bool isPalindrome(string s) {
    int i = 0, j = s.size() - 1;
    while (i < j) {
        while (i < j && !std::isalnum(s[i])) ++i;
        while (i < j && !std::isalnum(s[j])) --j;
        if (i > j || std::tolower(s[i]) != std::tolower(s[j])) {
            return false;
        }
        ++i, --j;
    }
    return true;
}
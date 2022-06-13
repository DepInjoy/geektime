/**
 * @brief   面试题 01.06. 字符串压缩
 *          
 *          字符串压缩。利用字符重复出现的次数，编写一种方法，实现基本的字符串压缩功能
 *          比如，字符串aabcccccaaa会变为a2b1c5a3。若“压缩”后的字符串没有变短，则返回原先的字符串。
 *          你可以假设字符串中只包含大小写英文字母（a至z）。
 *          链接：https://leetcode.cn/problems/compress-string-lcci/
 * 
 */

int countCompressLength(string S) {
    int compressLen = 0;
    for (int i = 0; i < S.size(); ++i) {
        if (i + 1 >= S.size() || S[i] != S[i+1]) {
            compressLen += 2;
        }
    }
    return compressLen;
}
string compressString(string S) {
    int compressLen = countCompressLength(S);
    if (compressLen >= S.size()) return S;
    
    string ans;
    ans.reserve(compressLen);
    int cnt = 0;
    for (int i = 0; i < S.size(); ++i) {
        ++cnt;
        if (i + 1 >= S.size() || S[i] != S[i+1]) {
            ans += S[i];
            ans += to_string(cnt);
            cnt = 0;
        }
    }
    return (ans.size() < S.size()) ? ans : S;
}
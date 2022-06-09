/**
 * @brief   01.03. URL化
 *          链接：https://leetcode.cn/problems/string-to-url-lcci/
 *          
 *          实现思想：
 *              1. 数出空格数量，然后填充字符
 * 
 */

// 数出空格数量，无需动态扩容
string replaceSpaces(string S, int length) {
    int spaceNum = 0;
    for (int i = 0; i < length; i++) {
        if (S[i] == ' ') spaceNum++;
    }

    string ans(length+2*spaceNum+1, ' ');
    int index = 0;
    for (int i = 0; i < length; i++) {
        if (S[i] == ' ') {
            ans[index++] = '%', ans[index++] = '2', ans[index++] = '0';
        } else {
            ans[index++] = S[i];
        }
    }
    return ans;
}

//  动态扩容，超出时间限制，应该有优化空间
string replaceSpaces(string S, int length) {
    string ans(S.substr(0, length));
    int index = 0;
    for (int i = length-1; i >= 0; i--) {
        if (S[i] == ' ') {
            ans[i] = '0';
            ans.insert(i, "%2");
        }
    }
    return ans;
}

string replaceSpaces(string S, int length) {
    stringstream ans;
    for (int i = 0; i < length; i++) {
        if (S[i] == ' ') {
            ans << "%20"; 
        } else {
            ans << S[i];
        }
    }
    return ans.str();
}
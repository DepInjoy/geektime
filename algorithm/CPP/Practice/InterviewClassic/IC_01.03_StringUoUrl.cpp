/**
 * @brief   01.03. URL化
 *          URL化。编写一种方法，将字符串中的空格全部替换为%20。
 *          假定该字符串尾部有足够的空间存放新增字符，并且知道字符串的“真实”长度
 *          （注：用Java实现的话，请使用字符数组实现，以便直接在数组上操作。）
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

// 题目假定字符串尾部有足够的空间存放新增字符
string replaceSpaces(string S, int length) {
    string ans(S);
    int i = S.size()-1, j = length -1;
    while(j >= 0) {
        if (S[j] == ' ') {
            ans[i--] = '0', ans[i--] = '2', ans[i--] = '%';
        } else {
            ans[i--] = S[j];
        }
        j--;
    }
    return ans.substr(i+1, S.size());
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
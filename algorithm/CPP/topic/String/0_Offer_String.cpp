/***/
#include <string>

using namespace std;

/**
 * @brief 剑指Offer-05:替换空格
 *          请实现一个函数，把字符串 s 中的每个空格替换成"%20"。
 * 
 *          https://leetcode.cn/problems/ti-huan-kong-ge-lcof/description/
 */

string replaceSpace(string s) {
    int spaceNum = 0;
    int originalLen = s.size();
    for (int i = 0; i < originalLen;++i) {
        if (s[i] == ' ') {
            ++spaceNum;
        }
    }

    if (spaceNum == 0) return s;

    int newlen = originalLen + spaceNum * 2;
    s.resize(newlen);
    for (int i = originalLen - 1, j= newlen - 1; i >= 0;--i) {
        if (s[i] == ' ') {
            s[j--] = '0';
            s[j--] = '2';
            s[j--] = '%';
        } else {
            s[j--] = s[i];
        }
    }
    return s;
}

/**
 * @brief 剑指 Offer-58 II:左旋转字符串
 *          字符串的左旋转操作是把字符串前面的若干个字符转移到字符串的尾部。
 *          请定义一个函数实现字符串左旋转操作的功能。
 *          比如，输入字符串"abcdefg"和数字2，该函数将返回左旋转两位得到的结果"cdefgab"。
 * 
 *      https://leetcode.cn/problems/zuo-xuan-zhuan-zi-fu-chuan-lcof/description
 * 
 */

string reverseLeftWords(string s, int n) {

    
}
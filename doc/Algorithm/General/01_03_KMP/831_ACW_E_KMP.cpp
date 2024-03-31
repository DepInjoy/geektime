/**
 * 831. KMP字符串
 * https://www.acwing.com/problem/content/833/
 * 
 * 题目描述：
 * 给定一个字符串S,以及一个模式串P所有字符串中只包含大小写英文字母以及阿拉伯数字
 * 模式串 P在字符串 S中多次作为子串出现
 * 求出模式串P在字符串 S中所有出现的位置的起始下标。
*/

#include <iostream>

const int N = 1000010;
char p[N], s[N];
int ne[N]; // next数组
int n, m;
int main() {
    std::cin >> m >> p >> n >> s;
    
    // 构建Next数组
    ne[0] = -1;
    for (int i = 1, j = -1; i < m; ++i) {
        while (j >= 0 && p[j+1] != p[i]) j = ne[j];
        if (p[j+1] == p[i]) j++;
        ne[i] = j;
    }
    
    for (int i = 0, j = -1; i < n; ++i) {
        while(j != -1 && s[i] != p[j+1]) j = ne[j];
        if (s[i] == p[j+1]) j++;
        if (j == m - 1) {
            std::cout << i - j << ' ';
            j = ne[j];
        }
    }
    
    return 0;
}
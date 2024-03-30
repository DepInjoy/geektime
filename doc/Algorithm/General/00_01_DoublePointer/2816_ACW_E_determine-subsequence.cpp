/**
 * 2816.判断子序列
 * https://www.acwing.com/problem/content/2818/
 * 
 * 题目描述：
 * 给定一个长度为 n的整数序列 a1,a2,…,an
 * 以及一个长度为 m的整数序列 b1,b2,…,bm
 * 请你判断a序列是否为b序列的子序列
 * 子序列指序列的一部分项按原有次序排列而得的序列
 * 例如序列 {a1,a3,a5}是序列 {a1,a2,a3,a4,a5}的一个子序列
*/

#include <iostream>

const int N = 100010;
int a[N], b[N];

int main() {
    int m, n;
    scanf("%d%d", &m, &n);
    for (int i = 0; i < m; ++i) scanf("%d", &a[i]);
    for (int i = 0; i < n; ++i) scanf("%d", &b[i]);
    
    int i = 0, j = 0;
    for (; i < n; ++i) {
        if (j < m && a[j] == b[i]) j++;
    }
    std::cout << ((j == m) ? "Yes" : "No");
    return 0;
}
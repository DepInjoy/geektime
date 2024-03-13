/**
 * 875. 快速幂
 * https://www.acwing.com/problem/content/877/
 * 
 * 给定 n组 ai,bi,pi，对于每组数据，求出 ai ^ bi mod pi的值。
 * 
 * 输入格式:
 * 第一行包含整数n。接下来n行，每行包含三个整数ai, bi, pi。
 * 
 * 输出格式:
 * 对于每组数据，输出一个结果，表示ai ^ bi mod pi的值。每个结果占一行。
 * 
 * 数据范围:
 * 1 ≤ n ≤ 100000, 1 ≤ ai,bi,pi ≤ 2 * 10^9
*/

#include <iostream>

typedef long long LL;

int qmi(int a, int b, int p) {
    int res = 1;
    while (b) {
        if (b & 1) res = (LL)res * a % p;
        b >>= 1;
        a = (LL)a * a % p;
    }
    return res;
}

int main() {
    int n;
    scanf("%d", &n);
    while (n--) {
        int a, b, p;
        scanf("%d%d%d", &a, &b, &p);
        std::cout << qmi(a, b, p) << std::endl;
    }
    return 0;
}
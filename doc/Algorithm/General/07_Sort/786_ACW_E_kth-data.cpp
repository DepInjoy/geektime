/**
 * 786.第k个数
 * https://www.acwing.com/problem/content/788/
 * 
 * 题目描述：
 * 给定一个长度为 n的整数数列，以及一个整数 k
 * 请用快速选择算法求出数列从小到大排序后的第 k个数
*/

#include <iostream>

const int N = 100010;
int data[N];

int quick_select(int data[], int l, int r, int k) {
    if (l == r) return data[l];
    
    int x = data[l + r >> 1], i = l -1, j = r + 1;
    while (i < j) {
        while(data[++i] < x);
        while(data[--j] > x);
        if (i < j) std::swap(data[i], data[j]);
    }
    
    if (j - l + 1 >= k) return quick_select(data, l, j, k);
    return quick_select(data, j + 1, r, k - (j - l + 1));
}

int main() {
    int n, k;
    scanf("%d%d", &n, &k);
    for (int i = 0; i < n; ++i) scanf("%d", &data[i]);
    
    std::cout << quick_select(data, 0, n - 1, k);
}
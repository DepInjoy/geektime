/**
 * 785.快速排序
 * https://www.acwing.com/problem/content/description/787/
*/

#include <iostream>

const int N = 100010;
int d[N];

void quick_sort(int l, int r) {
    if (l >= r) return;

    int i = l - 1, j = r + 1, x = d[l + r >> 1];
    while (i < j) {
        while(d[++i] < x);
        while(d[--j] > x);
        if (i < j) std::swap(d[i], d[j]);
    }
    quick_sort(l, i), quick_sort(j+1, r);
}

int main() {
    int n;
    scanf("%d", &n);
    
    for (int i = 0; i < n; ++i) {
        scanf("%d", &d[i]);
    }

    quick_sort(0, n-1);
    
    for (int i = 0; i < n; ++i) {
        printf("%d ", d[i]);
    }
    
    return 0;
}


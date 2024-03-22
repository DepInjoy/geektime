/**
 * 154. 滑动窗口
 * https://www.acwing.com/problem/content/description/156/
*/

#include <iostream>

const int N = 1000010;
int q[N], d[N];

int main() {
    int n, k;
    scanf("%d%d", &n, &k);
    for (int i = 0; i < n; ++i) {
        scanf("%d", &d[i]);
    }

    int hh = 0, tt = -1;
    for (int i = 0; i < n; ++i) {
        if (hh <= tt && q[hh] < i - k + 1) ++hh;
        while (hh <= tt && d[q[tt]] >= d[i]) --tt;
        q[++tt] = i;

        if (i >= k - 1) std::cout << d[q[hh]] << " ";
    }
    std::cout << std::endl;
    
    hh = 0, tt = -1;
    for (int i = 0; i < n; ++i) {
        if (hh <= tt && q[hh] < i - k + 1) ++hh;
        while (hh <= tt && d[q[tt]] <= d[i]) --tt;
        q[++tt] = i;

        if (i >= k - 1) std::cout << d[q[hh]] << " ";
    }
    
    return 0;
}
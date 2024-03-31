/**
 * 848. 有向图的拓扑序列
 * https://www.acwing.com/problem/content/850/
*/

#include <iostream>
#include <cstring>

const int N = 100010;
int h[N], e[N], ne[N], idx;
int d[N];
int q[N];
int m, n;

void add(int a, int b) {
    e[idx] = b, ne[idx] = h[a], h[a] = idx++;
}

bool topsort() {
    int hh = 0, tt = -1;
    // 将入度为0的节点入队
    for (int i = 1; i <= n; ++i) {
        if (d[i] == 0) q[++tt] = i;
    }

    while (hh <= tt) {
        int t = q[hh++];
        for (int i = h[t]; i != -1; i = ne[i]) {
            int j = e[i];
            if (--d[j] == 0) { // 删除i->j的边，入度为0进队
                q[++tt] = j;
            }
        }
    }
    return tt == n - 1;
}



int main() {
    memset(h, -1, sizeof(h));

    scanf("%d%d", &n, &m);
    for (int i = 0; i < m; ++i) {
        int a, b;
        scanf("%d%d", &a, &b);
        add(a, b);
        d[b]++;
    }


    if (topsort()) {
        for (int i = 0; i < n; ++i) {
            std::cout << q[i] << " ";
        }
    } else {
        std::cout << "-1" << std::endl;
    }
    
    return 0;
}
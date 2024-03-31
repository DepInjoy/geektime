/**
 * 847. 图中点的层次
 * https://www.acwing.com/problem/content/849/
 * 
 * 题目描述:
 * 给定一个 n个点 m条边的有向图，图中可能存在重边和自环
 * 所有边的长度都是1点的编号为 1∼n
 * 请你求出1号点到n号点的最短距离，如果从1号点无法走到n号点，输出−1
*/

#include <iostream>
#include <cstring>
#include <queue>

const int N = 100010;
int h[N], e[N], ne[N], idx;
int d[N];
int n, m;

void add(int a, int b) {
    e[idx] = b, ne[idx] = h[a], h[a] = idx++;
}

void bfs() {
    memset(d, -1, sizeof(d));
    
    std::queue<int> q;
    d[1] = 0;
    q.push(1);
    while (!q.empty()) {
        int t = q.front();
        q.pop();
        for (int i = h[t]; i != -1; i = ne[i]) {
            int j = e[i];
            if (d[j] == -1) {
                d[j] = d[t] + 1;
                q.push(j);
            }
        }
    }
}


int main() {
    memset(h, -1, sizeof(h));
    scanf("%d%d", &n, &m);
    for (int i = 0; i < m; ++i) {
        int a, b;
        scanf("%d%d", &a, &b);
        add(a, b);
    }
    
    bfs();
    std::cout << d[n] << std::endl;
    
    return 0;
}

/**
 * 849. Dijkstra求最短路 I
 * https://www.acwing.com/problem/content/851/
 * 
 * 题目描述:
 * 给定一个n个点m条边的有向图，图中可能存在重边和自环，所有边权均为正值
 * 请你求出1号点到 n号点的最短距离，如果无法从 1号点走到 n号点，则输出 −1
*/

#include <iostream>
#include <cstring>

const int N = 510;
int g[N][N];
int dist[N];
bool st[N];
int m, n;

int dijkstra() {
    memset(dist, 0x3f, sizeof(dist));
    
    dist[1] = 0;
    for (int i = 0; i < n - 1; ++i) {
        int t = -1;
        for (int j = 1; j <= n; ++j) {
            if (!st[j] && (t == -1 || dist[t] > dist[j])) {
                t = j;
            }
        }
        
        for (int j = 1; j <= n; ++j) {
            dist[j] = std::min(dist[j], dist[t] + g[t][j]);
        }
        st[t] = true;
    }
    
    return((dist[n] == 0x3f3f3f3f) ? -1 : dist[n]);
}

int main() {
    memset(g, 0x3f, sizeof(g));
    
    scanf("%d%d", &n, &m);
    while (m--) {
        int x, y, z;
        scanf("%d%d%d", &x, &y, &z);
        g[x][y] = std::min(g[x][y], z);
    }
    
    std::cout << dijkstra() << std::endl;
    
    return 0;
}
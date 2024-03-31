/**
 * 850. Dijkstra求最短路 II
 * https://www.acwing.com/problem/content/852/
 * 
 * 数据范围:
 *  1≤n,m≤1.5×105, 图中涉及边长均不小于0,且不超过 10000
 *  数据保证：如果最短路存在，则最短路的长度不超过 10^9
*/


#include <iostream>
#include <cstring>
#include <algorithm>
#include <queue>

typedef std::pair<int, int> PII;

const int N = 1000010;
int h[N], w[N], e[N], ne[N], idx;
int dist[N];
bool st[N];
int m, n;

void add(int a, int b, int c) {
    e[idx] = b, w[idx] = c, ne[idx] = h[a], h[a] = idx++;
}

int dijkstra() {
    memset(dist, 0x3f, sizeof(dist));
    
    dist[1] = 0;
    std::priority_queue<PII, std::vector<PII>, std::greater<PII>> heap;
    heap.push({0, 1});
    
    while (heap.size()) {
        auto t = heap.top();
        heap.pop();
        
        int nid = t.second, distance = t.first;
        if (st[nid]) continue;
        st[nid] = true;
        for (int i = h[nid]; i != -1; i = ne[i]) {
            int j = e[i];
            if (dist[j] > dist[nid] + w[i]) {
                dist[j] = dist[nid] + w[i];
                heap.push({dist[j], j});
            }
        }
    }
    
    return (dist[n] == 0x3f3f3f3f) ? -1 : dist[n];
}

int main() {
    memset(h, -1, sizeof(h));
    
    scanf("%d%d", &n, &m);
    while (m--) {
        int a, b, c;
        scanf("%d%d%d", &a, &b, &c);
        add(a, b, c);
    }
    
    std::cout << dijkstra() << std::endl;
    
    return 0;
}
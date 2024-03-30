/**
 * 
*/

#include <iostream>
#include <cstring>

const int N = 100010, M = N * 2;
int h[N], e[M], ne[M], idx;
bool st[N];
int n, ans = N;

void add(int a, int b) {
    e[idx] = b, ne[idx] = h[a], h[a] = idx++;
}

// 返回以u为节点的连通块儿的点数
int dfs(int u) {
    st[u] = true;
    
    // size:去除u节点连通块中点数的最大值
    int size = 0, sum = 0;
    for (int i = h[u]; i != -1; i = ne[i]) {
        int j = e[i];
        if (!st[j]) {
            int s = dfs(j);
            size = std::max(size, s);
            sum += s;
        }
    }
    size = std::max(size, n - 1 - sum);
    ans = std::min(ans, size);
    
    return sum + 1;
}


int main() {
    memset(h, -1, sizeof(h));

    scanf("%d", &n);
    for (int i = 0; i < n -1; ++i) {
        int a, b;
        scanf("%d%d", &a, &b);
        add(a, b), add(b, a);
    }
    
    dfs(1);
    std::cout << ans << std::endl;

    return 0;
}

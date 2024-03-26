/**
 * 240. 食物链
 * https://www.acwing.com/problem/content/242/
 * 
 * 到根结点的距离 % 3
 *     余1   可以吃根节点
 *     余2   可以被根节点
 *     余0   与根节点是同类
 */

#include <iostream>

const int N = 50010;
int p[N], d[N];

int find(int x) {
    if (p[x] != x) {
        int u = find(p[x]);
        d[x] += d[p[x]];
        p[x] = u; 
    }
    return p[x];
}


int main() {
    int n, k;
    scanf("%d%d", &n, &k);
    
    // 初始化
    for (int i = 1; i <= n; ++i) {
        p[i] = i, d[i] = 0;
    }
    
    int ans = 0;
    while (k--) {
        int op, x, y;
        scanf("%d%d%d", &op, &x, &y);
        
        if (x > n || y > n) {
            ++ans;
            continue;
        }

        int px = find(x), py = find(y);
        if (op == 1) {
            if (px != py) {
                p[px] = py;
                d[px] = d[y] - d[x];
            } else {
                if ((d[x] - d[y]) % 3) ans++;
            }
        } else {
            if (px != py) {
                p[px] = py;
                d[px] = d[y] - d[x] + 1;
            } else {
                if ((d[x] - d[y] - 1) % 3) ans++;
            }
        }
    }
    std::cout << ans << std::endl;

    return 0;
}
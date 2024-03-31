/**
 * 846. 树的重心
 * https://www.acwing.com/problem/content/848/
 * 
 * 题目描述:
 * 给定一颗树，树中包含n个结点(编号 1∼n)和n−1条无向边
 * 请你找到树的重心，并输出将重心删除后，剩余各个连通块中点数的最大值
 * 重心定义：重心是指树中的一个结点，如果将这个点删除后，剩余各个连通块中点数的最大值最小，那么这个节点被称为树的重心
 * 
 * 输出格式:输出一个整数m, 表示将重心删除后，剩余各个连通块中点数的最大值
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

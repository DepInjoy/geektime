/**
 * @brief   按照国际象棋的规则，皇后可以攻击与之处在同一行或同一列或同一斜线上的棋子。
 *          n 皇后问题 研究的是如何将n个皇后放置在n×n的棋盘上，并且使皇后彼此之间不能相互攻击。
 *          给你一个整数 n ，返回所有不同的 n 皇后问题 的解决方案。
 *          每一种解法包含一个不同的n皇后问题的棋子放置方案
 *          该方案中 'Q' 和 '.' 分别代表了皇后和空位。
 *          
 *          数据范围: 1≤n≤9
 *          
 *          链接：https://leetcode.cn/problems/n-queens
 *          链接: https://www.acwing.com/problem/content/845/
 */

#include <iostream>


// 逐行尝试
#if 0
const int N = 20;
char q[N][N];
bool col[N], dg[N], udg[N];

void dfs(const int u, const int n) {
    if (u == n) {
        for (int i = 0; i < n; i++) puts(q[i]);
        printf("\n");
        return;
    }

    for (int i = 0; i < n; i++) {
        if (!col[i] && !dg[i+u] && !udg[n-u+i]) {
            q[u][i] = 'Q';
            col[i] = dg[i+u] = udg[n-u+i] = true;
            dfs(u+1, n);
            q[u][i] = '.';
            col[i] = dg[i+u] = udg[n-u+i] = false;
        }
    }
}

int main() {
    int n;
    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            q[i][j] = '.';
        }
    }
    dfs(0, n);
}
#endif

// 逐个点尝试
#if 0
#include <iostream>

const int N = 20;
char q[N][N];
bool row[N], col[N], dg[N], udg[N];

void dfs(int x, int y, int s, const int n) {
    if (s > n) return;
    if (y == n) y = 0, x++;

    if (x == n) {
        if (s == n) {
            for (int i = 0; i < n; i++) puts(q[i]);
            printf("\n");
        }
        return;
    }
    
    q[x][y] = '.';
    dfs(x, y+1, s, n);

    if (!row[x] && !col[y] && !dg[x+y] && !udg[n+x-y]) {
        //std::cout << "x= " << x << " y= " << y << std::endl;
        q[x][y] = 'Q';
        row[x] = col[y] = dg[x+y] = udg[n+x-y] = true;
        dfs(x, y+1, s+1, n);
        q[x][y] = '.';
        row[x] = col[y] = dg[x+y] = udg[n+x-y] = false;
    }
}

int main() {
    int n;
    scanf("%d", &n);
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            q[i][j] = '.';
        }
    }

    dfs(0, 0, 0, n);
    return 0;
}
#endif

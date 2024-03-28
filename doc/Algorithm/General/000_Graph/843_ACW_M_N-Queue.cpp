/**
 * 843. n-皇后问题
 * https://www.acwing.com/problem/content/845/
 * 
 * 实现方法：
 *   1. DFS回溯搜索，有两种搜索方案:按行搜索和按格子搜索
 * 
 * n-皇后问题不错的解析博客
 * https://www.acwing.com/solution/content/2820/
*/

// 按行DFS搜索
#include <iostream>

const int N = 20;
char q[N][N];
bool col[N], dg[N], udg[N];

void dfs(int x, int n) {
    if (x == n) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                std::cout << q[i][j];
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
        return;
    }
    
    for (int y = 0; y < n; ++y) {
        if (!col[y] && !dg[x + y] && !udg[ n - x + y]) {
            q[x][y] = 'Q';
            col[y] = dg[x + y] = udg[n - x + y] = true;
            dfs(x+1, n);
            q[x][y] = '.';
            col[y] = dg[x + y] = udg[n - x + y] = false;
        }
    }
}

int main() {
    int n;
    scanf("%d", &n);
    
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            q[i][j] = '.';
        }
    }
    dfs(0, n);
    
    return 0;
}

// 按格子搜索
#include <iostream>

const int N = 10;
char q[N][N];
bool row[N], col[N], dg[2*N], udg[2*N];

void dfs(int x, int y, int cur, int n) {
    // 目前放置皇后数量超了，直接返回(因为一行最多有N个皇后，这个条件应该是不会触发的)
    if (cur > n) return;
    if (y == n) y = 0, ++x;

    if (x == n) {
        if (cur == n) { // 正好放了N个皇后
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    std::cout << q[i][j];
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
        return;
    }
    
    // 不放皇后
    q[x][y] = '.';
    dfs(x, y+1, cur, n);
    
    // 如果可以放置皇后，防止皇后继续尝试
    if (!row[x] && !col[y] && !dg[x + y] && !udg[n + x - y]) {
        q[x][y] = 'Q';
        row[x] = col[y] = dg[x+y] = udg[n+x-y] = true;
        dfs(x, y+1, cur+1, n);
        q[x][y] = '.';
        row[x] = col[y] = dg[x+y] = udg[n+x-y] = false;
    }
}

int main() {
    int n;
    scanf("%d", &n);
    
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            q[i][j] = '.';
        }
    }
    dfs(0, 0, 0, n);
    
    return 0;
}

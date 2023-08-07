# DFS
[842. 排列数字](https://www.acwing.com/problem/content/844/)
> 给定一个整数 n，将数字 1∼n 排成一排，将会有很多种排列方法。现在，请你按照字典序将所有的排列方法输出

```C++
#include <vector>
#include <iostream>

void dfs(int pos, const int n, std::vector<int> &path,
        std::vector<int>& st) {
    if (pos == n) {
        for (int i = 0 ; i < n; ++i) {
            std::cout << path[i] << " ";
        }
        std::cout << std::endl;
        return;
    }
    
    for (int i = 1; i <= n; ++i) {
        if (!st[i]) {
            path[pos] = i;
            st[i] = true;
            dfs(pos + 1, n, path, st);
            st[i] = false;
        }
    }
}

int main() {
    int n;
    scanf("%d", &n);
    
    std::vector<int> path(n), st(n+1);
    dfs(0, n, path, st);
    return 0;
}
```

[843. n-皇后问题](https://www.acwing.com/problem/content/845/)

[n-皇后问题(按行枚举或按每个元素枚举)](https://www.acwing.com/solution/content/2820/) 这个解析不错
## 逐行尝试
```C++
#include <vector>
#include <iostream>

void dfs(const int x, const int n, std::vector<std::vector<char>>& q,
        std::vector<bool>& col, std::vector<bool>& dg, std::vector<bool>& udg) {
    // 已经搜索了n行,输出
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
        // 剪枝(列, 对角线和反对角线都没有放皇后)放皇后尝试下一行
        if (!col[y] && !dg[x + y] && !udg[n - x + y]) {
            q[x][y] = 'Q';
            col[y] = dg[x + y] = udg[n - x + y] = true;
            dfs(x+1, n, q, col, dg, udg);
            // 恢复现场
            q[x][y] = '.';
            col[y] = dg[x + y] = udg[n - x + y] = false;
        }
    }
}

int main() {
    int n;
    scanf("%d", &n);
    
    std::vector<std::vector<char>> q(n, std::vector<char>(n, '.'));
    // 对角线和反对角线是否放皇后
    std::vector<bool> dg(2*n), udg(2*n);
    // 列是否放皇后
    std::vector<bool> col(n);
    dfs(0, n, q, col, dg, udg);
    return 0;
}
```
## 逐个尝试
```C++
#include <vector>
#include <iostream>

void dfs(int x, int y, int cnt, const int n, std::vector<std::vector<char>>& q,
            std::vector<bool>& row, std::vector<bool>& col,
            std::vector<bool>& dg, std::vector<bool>& udg) {
    if (y == n) y = 0, ++x;
    // 放了超过n个皇后
    if (cnt > n) return;
    // 最后一行且恰好放了n个皇后,才是有效结果
    if (x == n) {
        if (cnt == n) {
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


    // 分支1:[x][y]放皇后
    if (!row[x] && !col[y] && !dg[x + y] && !udg[n - x + y]) {
        q[x][y] = 'Q';
        row[x] = col[y] = dg[x + y] = udg[n - x + y] = true;
        dfs(x, y+1, cnt + 1, n, q, row, col, dg, udg);
        q[x][y] = '.';
        row[x] = col[y] = dg[x + y] = udg[n - x + y] = false;
    }

    // 分支2: [x][y]不放皇后
    dfs(x, y+1, cnt, n, q, row, col, dg, udg);
}

int main() {
    int n;
    scanf("%d", &n);
    
    std::vector<std::vector<char>> q(n, std::vector<char>(n, '.'));
    // 反对角线和对角线上是否存在皇后
    std::vector<bool> dg(2*n), udg(2*n);
    // 行和列上是否存在皇后
    std::vector<bool> col(n), row(n);
    dfs(0, 0, 0, n, q, row, col, dg, udg);
    return 0;
}
```

# BFS
[844. 走迷宫](acwing.com/problem/content/846/)

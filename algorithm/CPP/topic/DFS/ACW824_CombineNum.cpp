/**
 * @brief       842. 排列数字
 *              给定一个整数 n，将数字 1∼n 排成一排，将会有很多种排列方法。
 *              现在，请你按照字典序将所有的排列方法输出。
 *              输入格式 : 共一行，包含一个整数 n。
 *              输出格式:按字典序输出所有排列方案，每个方案占一行。
 *              数据范围:1≤n≤7
 *              链接：https://www.acwing.com/problem/content/844/
 */

#include <iostream>

const int N = 10;
int path[N];
bool st[N];

void dfs(const int u, const int n) {
    if (u == n) {
        for (int i = 0; i < n; i++)
            printf("%d ", path[i]);
        printf("\n");
        return;
    }
    

    for (int i = 1; i <= n; i++) {
        if (!st[i]) {
            path[u] = i;
            st[i] = true;
            dfs(u+1, n);
            st[i] = false;
        }
    }
}

int main() {
    int n;
    scanf("%d", &n);

    dfs(0, n);
    return 0;
}
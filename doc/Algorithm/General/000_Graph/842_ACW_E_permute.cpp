/**
 * 842. 排列数字
 * https://www.acwing.com/problem/content/description/844/
 * 
 * 给定一个整数 n，将数字 1∼n 排成一排，将会有很多种排列方法。
 * 请你按照字典序将所有的排列方法输出
*/

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
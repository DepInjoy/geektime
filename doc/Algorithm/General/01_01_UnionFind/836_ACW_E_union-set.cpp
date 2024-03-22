/**
 * 836. 合并集合
 * https://www.acwing.com/problem/content/838/
 * 
 * 题目描述:
 * 一共有n个数, 编号是 1∼n, 最开始每个数各自在一个集合中
 * 现在要进行m个操作，操作共有两种：
 *   M a b,将编号为a和b的两个数所在的集合合并,如果两个数已经在同一个集合中,则忽略这个操作；
 *   Q a b,询问编号为a和b的两个数是否在同一个集合中；
*/

#include <iostream>

const int N = 100010;
int d[N];

int find(int x) {
    if (d[x] != x) d[x] = find(d[x]);
    return d[x];
}

int main() {
    int m, n;
    scanf("%d%d", &n, &m);
    for (int i = 1; i <= n; ++i) d[i] = i;
    
    while (m--) {
        char op[2];
        int a, b;
        scanf("%s%d%d", op, &a, &b);

        if (op[0] == 'M') {
            d[find(a)] = find(b);
        } else {
            std::cout << (find(a) == find(b) ? "Yes" : "No") << std::endl;
        }
    }

    return 0;
}
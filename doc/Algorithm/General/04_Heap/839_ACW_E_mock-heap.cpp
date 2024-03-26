/**
 * 839. 模拟堆
 * https://www.acwing.com/problem/content/841/
*/

#include <iostream>
#include <cstring>

const int N = 100010;
// h: 表示堆中数据，下标从1开始
// hp: 表示堆中下标是i的元素是第几个插入的
// ph: 表示第k个插入的元素在堆中的元素的下标
// size表示堆中元素个数
int h[N], hp[N], ph[N], size;

void heap_swap(int a, int b) {
    std::swap(ph[hp[a]], ph[hp[b]]);
    std::swap(hp[a], hp[b]);
    std::swap(h[a], h[b]);
}

void down(int u) {
    int t = u;
    if (2*u <= size && h[2*u] < h[t]) t = 2*u;
    if (2*u+1 <= size && h[2*u+1] < h[t]) t = 2*u+1;
    if (t != u) {
        heap_swap(t, u);
        down(t);
    }
}

void up(int u) {
    while (u/2 && h[u] < h[u/2]) {
        heap_swap(u, u/2);
        u >>= 1;
    }
}

int main() {
    int n;
    scanf("%d", &n);
    
    // m表示插入的是第几个元素
    int x, k, m = 0;
    while (n--) {
        char op[4];
        scanf("%s", op);

        if (!strcmp(op, "I")) {
            scanf("%d", &x);
            h[++size] = x;
            ph[++m] = size;
            hp[size] = m;
            up(size);
        } else if (!strcmp(op, "PM")) {
            std::cout << h[1] << std::endl;
        } else if (!strcmp(op, "DM")) {
            heap_swap(1, size--);
            down(1);
        } else if (!strcmp(op, "D")) {
            scanf("%d", &k);
            // 注意，这里需要暂存一下ph[k],否则swap后值变化, 结果不对
            int u = ph[k];
            heap_swap(u, size--);
            down(u), up(u);
        } else if (!strcmp(op, "C")) {
            scanf("%d%d", &k, &x);
            h[ph[k]] = x;
            down(ph[k]), up(ph[k]);
        }
    }

    return 0;
}
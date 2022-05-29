/**
 * @brief   维护一个集合，初始时集合为空，支持如下几种操作：
 *              I x，插入一个数 x
 *              PM，输出当前集合中的最小值
 *              DM，删除当前集合中的最小值（数据保证此时的最小值唯一)
 *              D k，删除第 k 个插入的数；
 *              C k x，修改第 k 个插入的数，将其变为 x
 *          现在要进行 N 次操作，对于所有第 2 个操作，输出当前集合的最小值。
 *      
 *          输入格式
 *              第一行包含整数 N。
 *              接下来 N 行，每行包含一个操作指令，操作指令为 I x，PM，DM，D k 或 C k x 中的一种。
 *          输出格式
 *              对于每个输出指令 PM，输出一个结果，表示当前集合中的最小值。每个结果占一行。
 *          
 *          数据范围 1≤N≤10^5 −10^9≤x≤10^9，数据保证合法。
 */

#include <iostream>
#include <cstring>

using namespace std;

const int N = 100010;
int h[N], hp[N], ph[N], hsize = 0;

void heap_swap(int a, int b) {
    swap(hp[ph[a]], hp[ph[b]]);
    swap(ph[a], ph[b]);
    swap(h[a], h[b]);
}

void down(int i) {
    int t = i;
    if (2*i <= hsize && h[2*i] < h[t]) t =2*i;
    if (2*i+1 <= hsize && h[2*i+1] < h[t]) t = 2*i+1;
    if (t != i) {
        heap_swap(t, i);
        down(t);
    }
}

void up(int i) {
    while(i/2 && h[i/2] > h[i]) {
        heap_swap(i/2, i);
        i = i >> 1;
    }
}

int main() {
    int n, m = 0;;
    scanf("%d", &n);
    while(n--) {
        char op[10];
        int k, x;
        scanf("%s", op);
        if (!strcmp(op, "I")) {
            scanf("%d", &x);
            hp[++m] = ++hsize, ph[hsize] = m;
            h[hsize] = x;
            up(hsize);
        } else if (!strcmp(op, "D")) {
            scanf("%d", &k);
            k = hp[k];
            heap_swap(k, hsize--);
            down(k), up(k);
        } else if (!strcmp(op,"PM")) {
            printf("%d\n", h[1]);
        } else if (!strcmp(op, "DM")) {
            heap_swap(1, hsize);
            hsize--;
            down(1);
        } else if (!strcmp(op, "C")) {
            scanf("%d%d", &k, &x);
            k = hp[k];
            h[k] = x;
            down(k), up(k);
        }
    }
    return 0;
}
/**
 * @brief   838. 堆排序
 *          输入一个长度为 n 的整数数列，从小到大输出前 m 小的数。
 *          输入格式:第一行包含整数 n 和 m。第二行包含 n 个整数，表示整数数列。
 *          输出格式:共一行，包含 m 个整数，表示整数数列中前 m 小的数。
 *          数据范围:1≤m≤n≤105，1≤数列中元素≤109
 *          
 *          链接：https://www.acwing.com/problem/content/840/
 */

#include <iostream>

const int N = 100010;
int heap[N], size;

void down(int i) {
    int t = i;
    if (2*i <= size && heap[2*i] < heap[t]) t = 2*i;
    if (2*i+1 <= size && heap[2*i+1] < heap[t]) t = 2*i+1;
    if (t != i) {
        std::swap(heap[i], heap[t]);
        down(t);
    }
}

int main() {
    int m, n;
    scanf("%d %d", &n, &m);
    for (int i = 1; i <= n; i++) scanf("%d ", &heap[i]);
    size = n;

    // 建堆O(N)
    for (int i = size/2; i; i--) down(i);

    while (m--) {
        printf("%d ", heap[1]);
        heap[1] = heap[size--];
        down(1);
    }
    return 0;
}
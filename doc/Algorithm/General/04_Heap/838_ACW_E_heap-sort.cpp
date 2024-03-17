/**
 * 838. 堆排序
 * https://www.acwing.com/problem/content/840/
 * 
 * 1. 手写堆，可以自下而上构建堆其时间复杂度为O(N)
 * 
 * 2. 借助C++系统自带的std::priority_queue构建小顶堆
 *    建堆时间复杂度为O(NlogN)
*/

// 1. 手写堆，可以自下而上构建堆其时间复杂度为O(N)
#include <vector>
#include <iostream>

void down(std::vector<int>& h, int i, int size) {
    int t = i;
    if (2*i <= size && h[2*i] < h[i]) t = 2*i;
    if (2*i+1 <= size && h[2*i+1] < h[t]) t = 2*i+1;
    if (t != i) {
        std::swap(h[t], h[i]);
        down(h, t, size);
    }
}

int main() {
    int n, m;
    scanf("%d%d", &n, &m);

    std::vector<int> h(n+1);
    for (int i = 1; i <= n; ++i) scanf("%d", &h[i]);

    // 自底向上建堆,时间复杂度O(N)
    for (int i = n/2; i; --i) down(h, i, n);
    
    for (int i = 1; i <= m; ++i) {
        std::cout << h[1] << " ";
        h[1] = h[n--];
        down(h, 1, n);
    }
    return 0;
}
//-------------------------------------------------

// 2. 借助C++系统自带的std::priority_queue构建小顶堆
#include <iostream>
#include <queue>

int main() {
    int n, m;
    scanf("%d%d", &n, &m);
    
    // 小顶堆,时间复杂度为O(NlogN)
    std::priority_queue<int, std::vector<int>,
        std::greater<int>> h;
    for (int i = 0; i < n; ++i) {
        int val;
        scanf("%d", &val);
        h.push(val);
    }
    
    while (m--) {
        std::cout << h.top() << " ";
        h.pop();
    }
    return 0;
}
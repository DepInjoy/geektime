# 位运算
```C++
// 求n的第k位数字
n >> k & 1

// 返回n的最后一位1
lowbit(n) = n & -n
```

[801. 二进制中1的个数](https://www.acwing.com/problem/content/803/)
> 给定一个长度为n的数列，请你求出数列中每个数的二进制表示中1的个数。

```C++
#include <iostream>

int bits(int val) {
    int cnt = 0;
    while (val) {
        ++cnt;
        val -= (val & (~val + 1)); // val & -val
    }
    return cnt;
}

int main() {
    int n;
    scanf("%d", &n);
    while (n--) {
        int val;
        scanf("%d", &val);
        std::cout << bits(val) << " ";
    }
    return 0;
}
```

# 区间合并
[803. 区间合并](https://www.acwing.com/problem/content/805/)
> 给定 n个区间 [li,ri]，要求合并所有有交集的区间。注意如果在端点处相交，也算有交集。输出合并完成后的区间个数。例如：[1,3]和[2,6]可以合并为一个区间 [1,6]。

```C++
#include <vector>
#include <climits>
#include <algorithm>
#include <iostream>

typedef std::pair<int, int> PII;

void merge(std::vector<PII>& segs) {
    std::vector<PII> ans;
    std::sort(segs.begin(), segs.end());
    int start = INT_MIN, end = INT_MIN;
    for (auto& seg : segs) {
        if (end < seg.first) {
            if (start != INT_MIN) ans.push_back({start, end});
            start = seg.first, end = seg.second;
        } else {
            end = std::max(end, seg.second);
        }
    }
    
    if (start != INT_MIN) ans.push_back({start, end});
    segs = ans;
}

int main() {
    int n;
    scanf("%d", &n);
    
    std::vector<PII> segs(n);
    for (int i = 0; i < n; ++i) {
        int l, r;
        scanf("%d%d", &l, &r);
        segs[i] = {l, r};
    }
    
    merge(segs);
    std::cout << segs.size() << std::endl;
    return 0;
}
```
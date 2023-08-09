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

#  离散化
```C++
vector<int> alls; // 存储所有待离散化的值
sort(alls.begin(), alls.end()); // 将所有值排序
alls.erase(unique(alls.begin(), alls.end()), alls.end());   // 去掉重复元素

// 二分求出x对应的离散化的值
// 找到第一个大于等于x的位置
int find(int x) {
    int l = 0, r = alls.size() - 1;
    while (l < r) {
        int mid = l + r >> 1;
        if (alls[mid] >= x) r = mid;
        else l = mid + 1;
    }
    return r + 1; // 映射到1, 2, ...n
}
```

[802. 区间和](https://www.acwing.com/problem/content/804/)
> 假定有一个无限长的数轴，数轴上每个坐标上的数都是 0。
> 现在，我们首先进行 n 次操作，每次操作将某一位置 x 上的数加 c。
> 接下来，进行 m 次询问，每个询问包含两个整数 l 和 r，你需要求出在区间 [l,r] 之间的所有数的和。

```C++
#include <iostream>
#include <algorithm>
#include <vector>

using PII = std::pair<int, int>;


const int N = 300010;
int a[N], s[N]; // 分别存储插入后的数据和前缀和
std::vector<PII> add, query; // 分别存储插入和查询请求

int find(int pos, std::vector<int>& alls) {
    int l = 0, r = alls.size() - 1;
    while (l < r) {
        int mid = l + ((r - l) >> 1);
        if (alls[mid] >= pos) {
            r = mid;
        } else {
            l = mid + 1;
        }
    }
    return r + 1;
}

int main() {
    int n, m;
    scanf("%d%d", &n, &m);
    query.reserve(m);

    // 存储所有待离散化的值
    std::vector<int> alls;
    alls.reserve(n + 2 * m);
    for (int i = 0;i < n; ++i) {
        int x, c;
        scanf("%d%d", &x, &c);
        add.push_back({x,c});
        alls.push_back(x);
    }
    
    for (int i = 0; i < m; ++i) {
        int l, r;
        scanf("%d%d", &l, &r);
        query.push_back({l, r});
        alls.push_back(l), alls.push_back(r);
    }
    
    // 排序去重
    std::sort(alls.begin(), alls.end());
    alls.erase(std::unique(alls.begin(), alls.end()), alls.end());
    
    // 将数据映射到一个稠密空间，建立插入后数据a
    for (auto item : add) {
        int index = find(item.first, alls);
        a[index] += item.second;
    }
    
    // 构造前缀和
    for (int i = 1; i <= alls.size(); ++i) {
        s[i] = s[i-1] + a[i];
    }
    
    // 处理查询
    for (auto item : query) {
        int l = find(item.first, alls), r = find(item.second, alls);
        std::cout << s[r] - s[l-1] << std::endl;
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
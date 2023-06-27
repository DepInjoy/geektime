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
            if (start != seg.first)
                ans.push_back({seg.first, seg.second});
            start = seg.first, end = seg.second;
        } else {
            end = std::max(end, seg.second);
        }
    }
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
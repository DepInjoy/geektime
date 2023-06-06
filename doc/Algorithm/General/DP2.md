# 背包问题

# 线性DP
[898. 数字三角形](https://www.acwing.com/problem/content/900/)
> 给定一个如下图所示的数字三角形，从顶部出发，在每一结点可以选择移动至其左下方的结点或移动至其右下方的结点，一直走到底层，要求找出一条路径，使路径上的数字的和最大。

---
分析：
- 状态表示
    - 集合：从顶点到达`a[i][j]`所有的路径
    - 属性计算:路径和最大值max
- 状态分析，也就是进行集合划分，将集合可划分为左上方表示`dp[i-1][j-1]`和右上方表示为`dp[i-1][j]`

dp[i][j]表示从顶点到达`a[i][j]`的所有路径和的最大值。
状态计算方程：
```
dp[i][j] = max(dp[i-1][j-1] + a[i][j], dp[i-1][j] + a[i][j])
         = max(dp[i-1][j-1] , dp[i-1][j]) + a[i][j]
```

---

```C++
#include <climits>
#include <vector>
#include <iostream>

int main() {
    int n;
    scanf("%d", &n);
    std::vector<std::vector<int>> a(n, std::vector<int>(n));
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j <= i; ++j) {
            scanf("%d", &a[i][j]);
        }
    }
    
    std::vector<std::vector<int>> dp(n+1, std::vector<int>(n+1, INT_MIN));
    dp[1][1] = a[0][0];
    for (int i = 1; i < n; ++i) {
        for (int j = 0; j <=i; ++j) {
            dp[i+1][j+1] = std::max(dp[i][j+1], dp[i][j]) + a[i][j];
        }
    }
    
    int ans = INT_MIN;
    for (int i = 1; i <= n; ++i) {
        ans = std::max(dp[n][i], ans);
    }
    std::cout << ans;
    return 0;
}
```

[895. 最长上升子序列](https://www.acwing.com/problem/content/897/)
> 给定一个长度为N的数列，求数值严格单调递增的子序列的长度最长是多少？其中，1 ≤ N ≤ 1000

---
分析: 将动态规划的问题划分为两大部分：状态初始化和状态转移方程，其中状态转移方程又进一步划分为
- 状态表示`dp[i]`,将所有以第i个数字结尾的上升子序列作为待处理的**集合**，以及`dp[i]`存储的数值是**集合属性**：就是求最大长度`max`,可以采取集合+集合属性的方式表述为：
    - 集合：所有以第i个数字结尾的上升子序列
    - 集合属性：`max`(求最大长度)
- 状态计算，也就是集合的划分，以`a[i-1]`(也就是说满足集合条件的上一个数字)对集合进行分类，那么可以得到上一个数字可能是`a[0], a[1], a[2], ...., a[i-1]`


状态表示：`dp[i]`表示所有以第i个数字结尾的上升序列的最大长度

状态转移方程: `dp[i] = max(dp[i], dp[j]), a[j] < a[i], j=0,...,i-1`

状态初始化：`dp[i] = 1`(自身)

---

空间复杂度为O(N),时间复杂度为O(N^2)

```c++
#include <iostream>
#include <vector>

int main() {
    int n = 0;
    scanf("%d", &n);
    std::vector<int> v(n);
    for (int i = 0; i < n; ++i) {
        scanf("%d", &v[i]);
    }
    
    int ans = 1;
    std::vector<int> dp(n, 1);
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < i; ++j) {
            if (v[j] < v[i]) {
                dp[i] = std::max(dp[i], dp[j] + 1);
            }
        }
        ans = std::max(ans, dp[i]);
    }
    std::cout << ans;
    return 0;
}
```
<br/>


[896. 最长上升子序列 II](https://www.acwing.com/problem/content/898/)
> 给定一个长度为N的数列，求数值严格单调递增的子序列的长度最长是多少？其中，1 ≤ N ≤ 100000

---
分析：数据量增大，如果采用上面的O(N^2)的实现，将会超时。那么继续分析这里存在什么样的性质

假设存在1和3两个数字，如果第三个数字d可以接在3的后面，那么它肯定可以接在1的后面

<br/>
记dp[i]表示长度为i最长子序列结尾的数字的最小值，则dp[i]严格单调递增。接下来，进行证明

如果dp[i+1]<=dp[i],那么肯定存在一个比dp[i+1]小的值满足使得子序列长度为i，那么dp[i]便不再是长度为i最长子序列结尾的数字的最小值，和原定义矛盾，因此假设不成立。也就证明dp[i]严格单调递增。

---

空间复杂度为O(N),时间复杂度为O(NlogN)
```C++
#include <vector>
#include <iostream>

int main() {
    int n;
    scanf("%d", &n);
    std::vector<int> a(n);
    for (int i = 0; i < n; ++i) {
        scanf("%d", &a[i]);
    }
    
    int ml = 0;
    std::vector<int> dp(n);
    for (int i = 0; i < n; ++i) {
        // 寻找小于a[i]的最大值
        int l = 0, r = ml;
        while (l < r) {
            int mid = l + ((r + 1 - l) >> 1);
            if (dp[mid] < a[i]) {
                l = mid;
            } else {
                r = mid - 1;
            }
        }
        ml = std::max(ml, r+1);
        dp[r+1] = a[i];
    }

    std::cout << ml;
    return 0;
}
```

[897. 最长公共子序列](https://www.acwing.com/problem/content/899/)
> 给定两个长度分别为 N和 M的字符串 A和 B，求既是 A的子序列又是 B的子序列的字符串长度最长是多少。其中，1≤N,M≤1000

---

分析:也就是求既是A的子序列，又是B的子序列的最长字符串长度。

状态方程分为状态表示和状态计算量大部分大部分
- 状态表示dp[i, j]
    - 集合：所有`A[0~i]`和`B[0~j]`所有子序列集合
    - 属性计算：最大长度max
- 状态计算，也就是集合划分，可以划分为4大部分
    - 不含`A[i]`且不含`B[j]`的集合 `dp[i-1][j-1]`
    - 不含`A[i]`且含`B[j]`的集合，这部分可以分为含B[j]和不含B[j]两部分，分别对应于`dp[i-1][j]`和`dp[i-1][j-1]`,而`dp[i-1][j]`含了`dp[i-1][j-1]`由于是求最大值，出现重复也不影响结果。 `dp[i-1][j]`
    - 含`A[i]`且不含`B[j]`的集合  同上，此部分可表示为`dp[i][j-1]`
    - 含`A[i]`且含`B[j]`的集合, 如果`A[i]=B[j]`,则`dp[i-1][j-1]+1`

所以状态方程可以表示为
```
dp[i][j] = std::max(dp[i-1][j], dp[i][j-1]);
dp[i][j] = dp[i][j]
```

---

```C++
#include <iostream>
#include <vector>

int main() {
    int m, n;
    scanf("%d%d", &m, &n);
    char a[m], b[n];
    scanf("%s%s",a,b);
    
    std::vector<std::vector<int> > dp(m+1, std::vector<int>(n+1, 0));
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            dp[i+1][j+1] = std::max(dp[i][j+1], dp[i+1][j]);
            if (a[i] == b[j]) {
                dp[i+1][j+1] = std::max(dp[i+1][j+1], dp[i][j]+1);
            }
        }
    }
    std::cout << dp[m][n];
    return 0;
}
```

[902. 最短编辑距离](https://www.acwing.com/problem/content/904/)

---

分析：
- 状态表示`dp[i][j]`
    - 集合：将`a[1~i]`变成`b[1~j]`的所有操作
    - 属性：操作的最少次数

- 状态计算,也就是集合划分
    - 在`a[i]`删除一个字符，`dp[i-1][j] + 1`
    - 在`a[i]`插入一个字符 `dp[i][j-1] + 1`
    - 在`a[i]`替换一个字符 `dp[i-1][j-1]+0/1, a[i]==b[j]无需操作+0,否则需要1次替换操作`

- 状态初始化
    - `dp[i][0] = i`, 将`a[0~i]`删除`i`次变成`b[0]`
    - `dp[0][i] = i`, 插入i次从`a[0]`变成`b[0~i]`

---

```C++
#include <iostream>
#include <vector>

const int N = 1010;
char a[N], b[N];
int main() {
    int m, n;
    scanf("%d%s", &m, a);
    scanf("%d%s", &n, b);

    std::vector<std::vector<int>> dp(m+1, std::vector<int>(n+1, 0));
    for (int i = 0; i <= m; ++i) dp[i][0] = i;
    for (int i = 0; i <= n; ++i) dp[0][i] = i;
    
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            dp[i+1][j+1] = std::min(dp[i][j+1], dp[i+1][j]) + 1;
            dp[i+1][j+1] = std::min(dp[i+1][j+1], dp[i][j] + ((a[i] == b[j]) ? 0 : 1));
        }
    }
    std::cout << dp[m][n];
    return 0;
}
```

[899. 编辑距离](https://www.acwing.com/problem/content/901/)
```C++
#include <vector>
#include <iostream>
#include <string.h>

const int N = 1010, M = 10;
char a[N][M];
char b[M];
int dp[N][M];

int edit_distance(char a[], char b[]) {
    int m = strlen(a+1), n = strlen(b+1);
    for (int i = 0; i <= m; i++) dp[i][0] = i;
    for (int i = 0; i <= n; i++) dp[0][i] = i;
    
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            dp[i][j] = std::min(dp[i-1][j], dp[i][j-1]) + 1;
            dp[i][j] = std::min(dp[i][j], dp[i-1][j-1] + (a[i] == b[j] ? 0 : 1));
        }
    }
    return dp[m][n];
}

int main() {
    int n, m;
    scanf("%d%d", &n, &m);
    for(int i = 0; i < n; ++i) scanf("%s", a[i]+1);
    
    int ans = 0;
    while (m--) {
        int limit;
        scanf("%s%d", b+1, &limit);
        int ans = 0;
        for (int i = 0; i < n; ++i) {
            if (edit_distance(a[i], b) <= limit) {
                ++ans;
            }
        }
        std::cout << ans << std::endl;
    }
    return 0;
}
```

# 区间DP

# 计数类DP

# 数位统计DP

# 状态压缩DP

# 树形DP

# 记忆化搜索DP



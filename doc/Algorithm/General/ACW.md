[ACW算法基础课](https://www.acwing.com/activity/content/introduction/11/)

# 基础算法

| 专题     | 题目                                                         | 相关实现                                                     |
| -------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| 快排     | [785.快速排序](https://www.acwing.com/problem/content/description/787/)<br/>[786.第k个数](https://www.acwing.com/problem/content/788/) | [ACW 785.快速排序](07_Sort/785_ACW_E_quick_sort.cpp)<br/>[ACW 786.第k个数](07_Sort/786_ACW_E_kth-data.cpp) |
| 归并排序 | [787. 归并排序](https://www.acwing.com/problem/content/789/)<br/>[788. 逆序对的数量](https://www.acwing.com/problem/content/790/) | [ACW 787. 归并排序实现](07_Sort/787_ACW_E_merge_sort.cpp)<br/>[ACW 788. 逆序对的数量](07_Sort/788_ACW_E_number-pairs-in-reverse-order.cpp) |
| 二分     | [789.数的范围](https://www.acwing.com/problem/content/791/)<br/>[790. 数的三次方根](https://www.acwing.com/problem/content/792/) | [ACW 789.数的范围](05_BinarySearch/798_ACW_E_data-range.cpp)<br/>[ACW 790. 数的三次方根](05_BinarySearch/790_ACW_E_cube.cpp) |
| 双指针   | [799.最长连续不重复子序列](https://www.acwing.com/problem/content/801/)<br/>[800.数组元素的目标和](https://www.acwing.com/problem/content/802/)<br/>[2816.判断子序列](https://www.acwing.com/problem/content/2818/) | [ACW 799.最长连续不重复子序列](01_DoublePointer/799_ACW_E_longest-continuous-non-repeating-subsequence.cpp)<br/>[ACW 800.数组元素的目标和](01_DoublePointer/800_ACW_E_target-sum-array-elements.cpp)<br/>[ACW 2816.判断子序列](01_DoublePointer/2816_ACW_E_determine-subsequence.cpp) |
| 位运算   | [801. 二进制中1的个数](https://www.acwing.com/problem/content/803/) | [ACW 801. 二进制中1的个数](00_09_BitOp/ACW_801_E_number-of-1-in-binary.cpp) |


## 快速排序模板
```C++
void quick_sort(int q[], int l, int r) {
    if (l >= r) return;

    int i = l - 1, j = r + 1, x = q[l + r >> 1];
    while (i < j) {
        do i ++ ; while (q[i] < x);
        do j -- ; while (q[j] > x);
        if (i < j) swap(q[i], q[j]);
    }
    quick_sort(q, l, j), quick_sort(q, j + 1, r);
}
```



## 归并排序模板
```C++
void merge_sort(int q[], int l, int r) {
    if (l >= r) return;

    int mid = l + r >> 1;
    merge_sort(q, l, mid);
    merge_sort(q, mid + 1, r);

    int k = 0, i = l, j = mid + 1;
    while (i <= mid && j <= r)
        if (q[i] <= q[j]) tmp[k ++ ] = q[i ++ ];
        else tmp[k ++ ] = q[j ++ ];

    while (i <= mid) tmp[k ++ ] = q[i ++ ];
    while (j <= r) tmp[k ++ ] = q[j ++ ];

    for (i = l, j = 0; i <= r; i ++, j ++ ) q[i] = tmp[j];
}
```



## 二分模板

```C++
bool check(int x) {/* ... */} // 检查x是否满足某种性质

// 区间[l, r]被划分成[l, mid]和[mid + 1, r]时使用：
int bsearch_1(int l, int r) {
    while (l < r) {
        int mid = l + r >> 1;
        if (check(mid)) r = mid;    // check()判断mid是否满足性质
        else l = mid + 1;
    }
    return l;
}

// 区间[l, r]被划分成[l, mid - 1]和[mid, r]时使用：
int bsearch_2(int l, int r) {
    while (l < r) {
        int mid = l + r + 1 >> 1;
        if (check(mid)) l = mid;
        else r = mid - 1;
    }
    return l;
}
```




## 高精度

## 前缀和与差分



## 双指针模板
```C++
for (int i = 0, j = 0; i < n; i++){
    while (j < i && check(i, j)) j++;
    // 具体问题的逻辑
}

常见问题分类：
    (1) 对于一个序列，用两个指针维护一段区间
    (2) 对于两个序列，维护某种次序，比如归并排序中合并两个有序序列的操作
```



## 常用位运算

```C++
// 求n的第k位数字
n >> k & 1

// 返回n的最后一位1
lowbit(n) = n & -n
```



## 离散化

## 区间合并

# 数据结构

| 专题                 | 题目                                                         | 相关实现                                                     |
| -------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| 单调栈               | [830. 单调栈](https://www.acwing.com/problem/content/832/)   | [ACW 830. 单调栈](02_MonotonicStack/830_ACW_E_monotonic-stack.cpp) |
| 单调队列             | [154. 滑动窗口](https://www.acwing.com/problem/content/description/156/) | [ACW 154. 滑动窗口](06_SlidWindow/154_ACW_E_sliding-window.cpp) |
| 堆                   | [838. 堆排序](https://www.acwing.com/problem/content/840/)<br/>[839 模拟堆](https://www.acwing.com/problem/content/841/) | [ACW 838. 堆排序](04_Heap\838_ACW_E_heap-sort.cpp)<br/>[ACW 839 模拟堆](04_Heap/839_ACW_E_mock-heap.cpp) |
| Trie(字典树，前缀树) | [835. Trie字符串统计](https://www.acwing.com/problem/content/description/837/)<br/>[143.最大异或对](https://www.acwing.com/problem/content/145/) | [ACW 835. Trie字符串统计](01_00_Trie/835_ACW_E_string-stat.cpp) |
| 并查集               | [836. 合并集合](https://www.acwing.com/problem/content/838/)<br/>[837. 连通块中点的数量](https://www.acwing.com/problem/content/839/)<br/> [240. 食物链](https://www.acwing.com/problem/content/242/) | [ACW 836 合并集合](01_01_UnionFind/836_ACW_E_union-set.cpp)<br/>[ACW 837. 连通块中点的数量](01_01_UnionFind/837_ACW_E_number-points-in-connected-block.cpp)<br/>[ACW 240. 食物链](01_01_UnionFind/242_ACW_M_food-chain.cpp) |
| KMP                  | [831. KMP字符串](https://www.acwing.com/problem/content/833/) | [ACW 831. KMP字符串](01_03_KMP/831_ACW_E_KMP.cpp)            |



## 单调栈模板
常见模型：找出每个数左边离它最近的比它大/小的数
```C++
int tt = 0;
for (int i = 1; i <= n; i ++ ) {
    while (tt && check(stk[tt], i)) tt--;
    stk[++tt] = i;
}
```



## 单调队列模板
常见模型：找出滑动窗口中的最大值/最小值
```C++
int hh = 0, tt = -1;
for (int i = 0; i < n; i ++ ) {
    while (hh <= tt && check_out(q[hh])) hh ++ ;  // 判断队头是否滑出窗口
    while (hh <= tt && check(q[tt], i)) tt -- ;
    q[ ++ tt] = i;
}
```



## KMP模板

```C++
// s[]是长文本，p[]是模式串，n是s的长度，m是p的长度

// 求模式串的Next数组：
for (int i = 2, j = 0; i <= m; i ++ ) {
    while (j && p[i] != p[j + 1]) j = ne[j];
    if (p[i] == p[j + 1]) j ++ ;
    ne[i] = j;
}

// 匹配
for (int i = 1, j = 0; i <= n; i ++ ) {
    while (j && s[i] != p[j + 1]) j = ne[j];
    if (s[i] == p[j + 1]) j ++ ;
    if (j == m) {
        j = ne[j];
        // 匹配成功后的逻辑
    }
}
```

## Trie模板

```C++
// 0号点既是根节点，又是空节点
// son[][]存储树中每个节点的子节点
// cnt[]存储以每个节点结尾的单词数量
int son[N][26], cnt[N], idx;

// 插入一个字符串
void insert(char *str) {
    int p = 0;
    for (int i = 0; str[i]; i ++ ) {
        int u = str[i] - 'a';
        if (!son[p][u]) son[p][u] = ++idx;
        p = son[p][u];
    }
    cnt[p] ++ ;
}

// 查询字符串出现的次数
int query(char *str) {
    int p = 0;
    for (int i = 0; str[i]; i ++ ) {
        int u = str[i] - 'a';
        if (!son[p][u]) return 0;
        p = son[p][u];
    }
    return cnt[p];
}
```



## 并查集模板

1. 朴素并查集

```C++
int p[N]; //存储每个点的祖宗节点
// 返回x的祖宗节点
int find(int x) {
    if (p[x] != x) p[x] = find(p[x]);
    return p[x];
}

// 初始化，假定节点编号是1~n
for (int i = 1; i <= n; i ++ ) p[i] = i;

// 合并a和b所在的两个集合
p[find(a)] = find(b);
```

2. 维护size的并查集

```C++
// p[]存储每个点的祖宗节点
// size[]只有祖宗节点的有意义，表示祖宗节点所在集合中的点的数量
int p[N], size[N];

// 返回x的祖宗节点
int find(int x) {
    if (p[x] != x) p[x] = find(p[x]);
    return p[x];
}

// 初始化，假定节点编号是1~n
for (int i = 1; i <= n; i++ ) {
    p[i] = i, size[i] = 1;
}

// 合并a和b所在的两个集合：
size[find(b)] += size[find(a)];
p[find(a)] = find(b);
```

3. 维护到祖宗节点距离的并查集
```C++
int p[N], d[N];
//p[]存储每个点的祖宗节点, d[x]存储x到p[x]的距离

// 返回x的祖宗节点
int find(int x) {
    if (p[x] != x) {
        int u = find(p[x]);
        d[x] += d[p[x]];
        p[x] = u;
    }
    return p[x];
}

// 初始化，假定节点编号是1~n
for (int i = 1; i <= n; i ++ ) {
    p[i] = i;
    d[i] = 0;
}

// 合并a和b所在的两个集合：
p[find(a)] = find(b);
d[find(a)] = distance; // 根据具体问题，初始化find(a)的偏移量
```



## 堆模板
```C++
// h[N]存储堆中的值,下标从1开始, h[1]是堆顶，x的左儿子是2x, 右儿子是2x + 1
// ph[k]存储第k个插入的点在堆中的位置
// hp[k]存储堆中下标是k的点是第几个插入的
int h[N], ph[N], hp[N], size;

// 交换两个点，及其映射关系
void heap_swap(int a, int b) {
    swap(ph[hp[a]],ph[hp[b]]);
    swap(hp[a], hp[b]);
    swap(h[a], h[b]);
}

void down(int u) {
    int t = u;
    if (u * 2 <= size && h[u * 2] < h[t]) t = u * 2;
    if (u * 2 + 1 <= size && h[u * 2 + 1] < h[t]) t = u * 2 + 1;
    if (u != t) {
        heap_swap(u, t);
        down(t);
    }
}

void up(int u) {
    while (u / 2 && h[u] < h[u / 2]) {
        heap_swap(u, u / 2);
        u >>= 1;
    }
}

// 自底向上建堆，时间复杂度O(N)
for (int i = n / 2; i; i -- ) down(i);
```

# 动态规划

| 专题                                                         | 题目                                                         | 相关实现                                                     |
| ------------------------------------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
| 背包问题<br/>1. 01背包问题<br/>2. 完全背包问题<br/>3. 多重背包问题<br/><br/>4. 分组背包问题 | <br/>[2. 01背包问题](https://www.acwing.com/problem/content/2/)<br/>[3. 完全背包问题](https://www.acwing.com/problem/content/3/)<br/>[4. 多重背包问题 I](https://www.acwing.com/problem/content/4/)<br/>[5. 多重背包问题 II](https://www.acwing.com/problem/content/5/)<br/>[9. 分组背包问题](https://www.acwing.com/problem/content/9/) | <br/>[ACW 2. 01背包实现](02_DP/02_ACW_E_01-bag.cpp)<br/>[ACW 3. 完全背包实现](02_DP/03_ACW_E_full-bag.cpp)<br/>[ACW 4. 多重背包实现](02_DP/04_ACW_E_multi-bag.cpp)<br/><br/>[ACW 9. 分组背包实现](02_DP/09_ACW_M_group-bag.cpp) |
|                                                              | \*                                                           |                                                              |
|                                                              |                                                              |                                                              |



## 背包问题

### 01背包问题
[2. 01背包问题](https://www.acwing.com/problem/content/2/)
> 有N件物品和一个容量是V的背包。每件物品只能使用一次。第i件物品的体积是 vi，价值是 wi。求解将哪些物品装入背包，可使这些物品的总体积不超过背包容量，且总价值最大。输出最大价值。

状态表示dp[i, j]
- 集合：从前i个物品中选择体积小于等于j的所有物品集合
- 属性计算:物品价值最大值`max`

状态计算:
```
-------------------------------------------
|      sub1        |         sub2         |
-------------------------------------------

sub1：不含第i个物品, dp[i-1][j]
sub2: 含第i个物品, dp[i-1][j-vi] + wi

由此，状态转移方程为:dp[i][j] = max(dp[i-1][j], dp[i-1][j-wi] + vi)
```

状态转移方程:
$$
\begin{array}{l}
dp[i][j] = max(dp[i-1][j], dp[i-1][j-v[i]] + w[i])
\end{array}
$$
对于j的遍历，从大到小进行更新，可以进行空间压缩到一维dp[j]
$$
\begin{array}{l}
dp[j] = max(dp[j], dp[j-v[i]] + w[i])
\end{array}
$$

```C++
#include <vector>
#include <iostream>

int main() {
    int m, n;
    scanf("%d%d", &m, &n);
    std::vector<int> v(m), w(m);
    for (int i = 0; i < m; ++i) scanf("%d%d", &v[i], &w[i]);
    
    std::vector<std::vector<int>> dp(m+1, std::vector<int>(n+1, 0));
    for (int i = 0; i < m; ++i) {
        for (int j = 1; j <= n; ++j) {
            dp[i+1][j] = dp[i][j];
            if (j >= v[i]) dp[i+1][j] = std::max(dp[i+1][j], dp[i][j-v[i]] + w[i]);
        }
    }
    std::cout << dp[m][n];
    return 0;
}
```
滚动数组优化，降维减少空间复杂度
```C++
#include <vector>
#include <iostream>

int main() {
    int m, n;
    scanf("%d%d", &m, &n);
    std::vector<int> v(m), w(m);
    for (int i = 0; i < m; ++i) scanf("%d%d", &v[i], &w[i]);
    
    std::vector<int> dp(n+1, 0);
    for (int i = 0; i < m; ++i) {
        for (int j = n; j >= v[i]; --j) {
            dp[j] = std::max(dp[j], dp[j-v[i]] + w[i]);
        }
    }
    std::cout << dp[n];
    return 0;
}
```

### 完全背包问题
[3. 完全背包问题](https://www.acwing.com/problem/content/3/)
> 有 N 种物品和一个容量是 V 的背包，每种物品都有无限件可用。第 i 种物品的体积是 vi，价值是 wi。求解将哪些物品装入背包，可使这些物品的总体积不超过背包容量，总价值最大。输出最大价值。

状态表示dp[i, j]
- 集合：从前i个物品中选择提及小于等于j的所有物品集合
- 属性计算:物品价值最大值`max`

状态计算:
```
-------------------------------------------------------
|  sub0  | sub1  |  sub2  |  ... |  subk  |  ....     |
-------------------------------------------------------

sub0：不含第i个物品, dp[i-1][j]
sub1: 含1个第i个物品, dp[i-1][j-vi] + wi
sub1: 含1个第i个物品, dp[i-1][j-2*vi] + wi*2
...
sub1: 含1个第i个物品, dp[i-1][j-k*vi] + wi*k
....

由此，状态转移方程为:
dp[i][j] = max(dp[i-1][j], dp[i-1][j-vi] + wi, dp[i-1][j-2*vi] +2wi, .....,  dp[i-1][j-k*vi] +k*wi) -- 公式1

用j-vi替换上述方程中的j可以得到
dp[i][j-vi] = max(dp[i-1][j-vi], dp[i-1][j-2*vi] + wi, dp[i-1][j-3*vi] +2wi, .....,  dp[i-1][j-(k+1)*vi] +k*wi) -- 公式2

因此公式1可以转化为：
dp[i][j] = max(dp[i-1][j], dp[i][j-vi]+wi)
```

因此，状态转移方程:
$$
\begin{array}{l}
dp[i][j] = max(dp[i-1][j], dp[i][j-v[i]] + w[i])
\end{array}
$$
和01背包类似，将j从0到m遍历，可以进行空间压缩
$$
\begin{array}{l}
dp[j] = max(dp[j], dp[j-v[i]]+w[i])
\end{array}
$$

```C++
#include <vector>
#include <iostream>

int main() {
    int m, n;
    scanf("%d%d", &m, &n);
    std::vector<int> v(m), w(m);
    for (int i = 0; i < m; ++i) scanf("%d%d", &v[i], &w[i]);
    
    std::vector<int> dp(n+1, 0);
    for (int i = 0; i < m; ++i){
        for (int j = v[i]; j <= n; ++j) {
            dp[j] = std::max(dp[j], dp[j-v[i]] + w[i]);
        }
    }
    std::cout << dp[n];
    return 0;
}
```

### 多重背包问题
[4. 多重背包问题 I]()
> 有N种物品和一个容量是V的背包。第i种物品最多有si件，每件体积是vi，价值是wi。求解将哪些物品装入背包，可使物品体积总和不超过背包容量，且价值总和最大。输出最大价值。
> 
> 0 < N; V ≤ 100; 0 < vi, wi , si ≤ 100

状态表示dp[i, j]
- 集合：从前i个物品中选择提及小于等于j的所有物品集合
- 属性计算:物品价值最大值`max`

状态计算:
```
------------------------------------------------------------
|  sub0  | sub1  |  ... |  subk  |  .... |  subs  |
------------------------------------------------------------

sub0：不含第i个物品, dp[i-1][j]
sub1: 含1个第i个物品, dp[i-1][j-vi] + wi
...
subk: 含k个第i个物品, dp[i-1][j-k*vi] + wi*k
....
subs: 含s个第i个物品, dp[i-1][j-s*vi] + wi*s

由此，状态转移方程为:
dp[i][j] = max(dp[i-1][j], dp[i-1][j-vi] + wi, .....,  dp[i-1][j-k*vi] +k*wi, dp[i-1][j-s*vi] +s*wi) -- 公式1
```

```C++
#include <vector>
#include <iostream>

int main() {
    int m, n;
    scanf("%d%d", &m, &n);

    std::vector<int> dp(n+1, 0);
    for (int i = 0; i < m; ++i) {
        int v, w, s;
        scanf("%d%d%d", &v, &w, &s);
        for (int j = n; j >= v; --j) {
            for (int k = 1; k <= s && j >= k*v; ++k) {
                dp[j] = std::max(dp[j], dp[j-k*v] + k*w);
            }
        }
    }
    std::cout << dp[n];
    return 0;
}
```

[5. 多重背包问题 II](https://www.acwing.com/problem/content/5/)
> 0<N≤1000, 0<V≤2000, 0<vi,wi,si≤2000

```
数字1-6可以用 1,2，3进行表示，
1 = 1
2 = 2
3 = 3
4 = 1 + 3
5 = 2 + 3
6 = 1 + 2 + 3

因此可以将上述实现拆分为MN * (log(S)向上取整)

假设K=log(S)，可以将其拆分为， 1， 2, ...., 2^k, S - 2^k个01背包问题
```
```C++
#include <vector>
#include <iostream>

struct Good {
    int v;
    int w;
};

int main() {
    int m, n;
    scanf("%d%d", &m, &n);
    
    std::vector<Good> goods;
    for (int i = 0; i < m; ++i) {
        int v, w, s;
        scanf("%d%d%d", &v, &w, &s);
        for (int k = 1; k <= s; k *=2) {
            s -= k;
            goods.push_back({k*v, k*w});
        }
        if (s > 0) goods.push_back({s*v, s*w});
    }
    
    std::vector<int> dp(n+1, 0);
    for (auto& iter : goods) {
        for (int j = n; j >= iter.v; --j) {
            dp[j] = std::max(dp[j], dp[j-iter.v] + iter.w);
        }
    }
    std::cout << dp[n];
    return 0;
}
```

### 分组背包问题
[9. 分组背包问题](https://www.acwing.com/problem/content/9/)
> 有 N 组物品和一个容量是 V 的背包。每组物品有若干个，同一组内的物品最多只能选一个。每件物品的体积是 vij，价值是 wij，其中 i 是组号，j 是组内编号。求解将哪些物品装入背包，可使物品总体积不超过背包容量，且总价值最大。
```C++
#include <vector>
#include <iostream>

int main() {
    int m, n;
    scanf("%d%d", &m, &n);
    
    std::vector<int> dp(n+1, 0);
    for (int i = 0; i < m; ++i) {
        int s;
        scanf("%d", &s);
        std::vector<int> v(s), w(s);
        for (int k = 0; k < s; ++k) {
            scanf("%d%d", &v[k], &w[k]);
        }
        
        for (int j = n; j >= 0; --j) {
            for (int k = 0; k < s; ++k) {
                if (j >= v[k]) dp[j] = std::max(dp[j], dp[j - v[k]] + w[k]);
            }
        }
        
    }
    std::cout << dp[n];
    return 0;
}
```

## 线性DP
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

## 区间DP
[282. 石子合并](https://www.acwing.com/problem/content/284/)
> 设有 N 堆石子排成一排，其编号为 1,2,3,…,N。每堆石子有一定的质量，可以用一个整数来描述，现在要将这 N 堆石子合并成为一堆。每次只能合并相邻的两堆，合并的代价为这两堆石子的质量之和，合并后与这两堆石子相邻的石子将和新堆相邻，合并时由于选择的顺序不同，合并的总代价也不相同。
>
> 问题是：找出一种合理的方法，使总的代价最小，输出最小代价。

- 状态表示dp[i, j]
    - 集合: 编号从i到j合并到一起的所有操作
    - 属性: 最小代价`min`

- 状态计算
```
----------------------------------------------------------------------------
|  subi  | sub(i+1)  |  sub(i+2)  |  ... |  sub(i+k)  |  .... |  sub(j-1)  |
----------------------------------------------------------------------------

subi：将i到i合并到起一些的操作 dp[i][i]
sub(i+1)：将i到i+1合并到起一些的操作 dp[i][i+1]
...
sub(i+k): 将i到(i+k)合并到一起的操作, dp[i][i+k]
...
sub(j-1): 将i到(j-1)合并到一起的操作, dp[i][j-1]

对于sub(i+k)，由于题目要求是合并相邻两堆，因此可以分解为
i-------------------k k+1-----------------------j
     dp[i][k]                    dp[k+1][j]          sum(i...j)

因此，有
dp[i][k] + dp[k+1][j] + s[j] - s[i-1]
```

```C++
#include <vector>
#include <iostream>
#include <climits>

int main() {
    int n;
    scanf("%d", &n);
    std::vector<int> s(n+1, 0);
    for (int i = 1; i <= n; ++i) {
        scanf("%d", &s[i]);
        s[i] += s[i-1];
    }
    
    std::vector<std::vector<int>> dp(n+1, std::vector<int>(n+1));
    for (int len = 2; len <= n; ++len) {
        for (int i = 1; i + len - 1 <= n; ++i) {
            int j = i + len - 1;
            dp[i][j] = INT_MAX;
            for (int k = i; k < j; ++k) {
                dp[i][j] = std::min(dp[i][j], dp[i][k] + dp[k+1][j] + s[j] - s[i-1]);
            }
        }
    }
    std::cout << dp[1][n];
    return 0;
}
```
## 计数类DP
[900. 整数划分](https://www.acwing.com/problem/content/902/)
> 一个正整数 n 可以表示成若干个正整数之和，形如：n=n1+n2+…+nk，其中n1≥n2≥…≥nk,k≥1。我们将这样的一种表示称为正整数 n 的一种划分。现在给定一个正整数 n，请你求出 n 共有多少种不同的划分方法。

<b><font color="orange">完全背包解法</font></b>
- 状态表示dp[i][j]
    - 集合：用整数1~i正好表示j的所有方法的集合
    - 属性：数量
- 状态计算
```
-------------------------------------------------------------------
| sub0  | sub(1)  |  sub(2)  |  ... |  sub(k)  |  .... |  sub(s)  |
-------------------------------------------------------------------
sub0: 采用用一个i正好表示j   dp[i-1][j]
sub1: 采用用一个i正好表示j   dp[i-1][j-i]
sub2: 采用2个i正好表示j     dp[i-1][j-2i]
......

subk: 采用k个i正好表示j     dp[i-1][j-k*i]
...
subs: 采用s个i正好表示j     dp[i-1][j-s*i]

dp[i][j] =   dp[i-1][j] + dp[i-1][j-i] + dp[i-1][j-2i] +  .... + dp[i-1][j-s*i]
dp[i][j-i] =            + dp[i-1][j-i] + dp[i-1][j-2*i] + .... + dp[i-1][j-s*i]

dp[i][j] = dp[i-1][j] + dp[i][j-i]

进行降维,从小到大遍历，可以得到

dp[j] = dp[j] + dp[j-i]
```

```C++
#include <vector>
#include <iostream>

int main() {
    const int mod = 1e9 + 7;
    int n;
    scanf("%d", &n);
    
    std::vector<int> dp(n+1);
    dp[0] = 1;
    for (int i = 1; i <= n; ++i) {
       for (int j = i; j <= n; ++j) {
           dp[j] = (dp[j] + dp[j-i]) % mod;
       } 
    }
    std::cout << dp[n];
    return 0;
}
```


<b><font color="orange">其他解法</font></b>

- 状态表示dp[i][j]
    - 集合：总和是i且正好表示成j个数的所有方法的集合
    - 属性：数量
- 状态计算
```
-------------------
|  sub0  |  sub1  |
-------------------

sub0: 最小值是1    dp[i-1][j-1]
sub1: 最小值大于1  dp[i-j][j] (表示中的j个数每个减1依然是可行的方法，因此转化为总和恰好是i-j且正好表示成j个数的方法)

dp[i][j] = dp[i-1][j-1] + dp[i-j][j]
```

```C++
#include <vector>
#include <iostream>

int main() {
    const int mod = 1e9 + 7;
    int n;
    scanf("%d", &n);

    std::vector<std::vector<int>> dp(n+1, std::vector<int>(n+1));
    dp[0][0] = 1;
    for (int i = 1; i <= n; ++i) {
        for (int j = i; j >= 1; --j) {
            dp[i][j] = (dp[i-1][j-1] + dp[i-j][j]) % mod;
        }
    }

    int ans = 0;
    for (int i = 1; i <= n; ++i) {
        ans = (ans + dp[n][i]) % mod;
    }
    std::cout << ans << std::endl;
    return 0;
}
```

## 数位统计DP
[338. 计数问题](https://www.acwing.com/problem/content/340/)
> 给定两个整数a和 b，求a和b之间的所有数字中 0∼9的出现次数。
## 状态压缩DP

## 树形DP

## 记忆化搜索DP
[901. 滑雪](https://www.acwing.com/problem/content/903/)
> 给定一个 R行 C列的矩阵，表示一个矩形网格滑雪场。矩阵中第 i行第 j列的点表示滑雪场的第 i行第 j列区域的高度。一个人从滑雪场中的某个区域内出发，每次可以向上下左右任意一个方向滑动一个单位距离。当然，一个人能够滑动到某相邻区域的前提是该区域的高度低于自己目前所在区域的高度。

分析:
- 状态表示dp[i][j]
    - 集合:从第i行j列开始滑的所有路径
    - 属性：最长滑雪轨迹`max`
- 状态计算
```
---------------------------
|    sub1    |    sub2    |
---------------------------
|    sub3    |    sub4    |
---------------------------

sub1:从i行j列向上滑所有的路径, dp[i][j-1] + 1
sub2:从i行j列向下滑所有的路径, dp[i][j+1] + 1
sub3:从i行j列向左滑所有的路径, dp[i-1][j] + 1
sub4:从i行j列向右滑所有的路径, dp[i+1][j] + 1
```

```C++
#include <vector>
#include <iostream>

int dx[4] = {-1, 0, 1, 0};
int dy[4] = {0, -1, 0, 1};

int search(int x, int y, int m, int n, const std::vector<std::vector<int>>& h,
        std::vector<std::vector<int>>& dp) {
    if (dp[x][y] != -1) return dp[x][y];
    
    dp[x][y] = 1;
    for (int i = 0; i < 4; ++i) {
        int a = x + dx[i], b = y + dy[i];
        if (a >= 1 && a <= m && b >= 1 && b <= n && h[a][b] < h[x][y]) {
            dp[x][y] = std::max(dp[x][y], search(a, b, m , n, h, dp) + 1);
        }
    }
    return dp[x][y];
}

int main() {
    int m, n;
    scanf("%d%d", &m, &n);
    
    std::vector<std::vector<int>> h(m+1, std::vector<int>(n+1));
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            scanf("%d", &h[i][j]);
        }
    }
    
    int ans = 0;
    std::vector<std::vector<int>> dp(m+1, std::vector<int>(n+1, -1));
    for (int i = 1; i <= m; ++i) {
        for(int j = 1; j <= n; ++j) {
            ans = std::max(ans, search(i, j, m, n, h, dp));
        }
    }
    std::cout << ans << std::endl;
    return 0;
}
```

# 贪心
| 专题     | 题目                                                         | 相关实现                                                     |
| -------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| 推公式     | [125.耍杂技的牛](https://www.acwing.com/problem/content/127/) | [ACW 125.耍杂技的牛](06_Greedy/125_ACW_M_acrobatic-cow.cpp)         |



# 搜索和图论
| 专题     | 题目                                                         | 相关实现                                                     |
| -------- | ------------------------------------------------------------ | ------------------------------------------------------------ |
| DFS     | [842.排列数字](https://www.acwing.com/problem/content/description/844/)<br/>[843.n-皇后问题](https://www.acwing.com/problem/content/845/) | [ACW 842.排列数字](000_Graph/842_ACW_E_permute.cpp)<br/>[ACW 843.n-皇后问题](000_Graph/843_ACW_M_N-Queue.cpp)         |
| BFS |  |  |
| 树与图的深度优先遍历 | [846. 树的重心](https://www.acwing.com/problem/content/848/) | [ACW 846. 树的重心](000_Graph/846_ACW_E_center-of-tree.cpp) |
| 树与图的广度优先遍历 | [847. 图中点的层次](https://www.acwing.com/problem/content/849/) | [ACW 847. 图中点的层次](000_Graph/847_ACW_E_level-of-graph.cpp) |
| 拓扑排序 | [848.有向图的拓扑序列](https://www.acwing.com/problem/content/850/) | [ACW 848.有向图的拓扑序列](000_Graph/848_ACW_E_dag-topological-order.cpp) |
| Dijkstra | [849. Dijkstra求最短路 I](https://www.acwing.com/problem/content/851/)<br/>[850.Dijkstra求最短路 II](https://www.acwing.com/problem/content/852/) | [ACW 849. Dijkstra求最短路 I](000_Graph\849_ACW_E_dijkstra-i.cpp)<br/>[ACW 850.Dijkstra求最短路 II](000_Graph/850_ACW_E_dijkstra-ii.cpp) |





## 树与图的存储

树是一种特殊的图，与图的存储方式相同。对于无向图中的边ab，存储两条有向边a->b, b->a。因此我们可以只考虑有向图的存储。

1. 邻接矩阵：`g[a][b]`存储边a->b

2. 邻接表：

   ```C++
   // 对于每个点k，开一个单链表，存储k所有可以走到的点。h[k]存储这个单链表的头结点
   int h[N], e[N], ne[N], idx;
   
   // 添加一条边a->b
   void add(int a, int b) {
       e[idx] = b, ne[idx] = h[a], h[a] = idx ++ ;
   }
   
   // 初始化
   idx = 0;
   memset(h, -1, sizeof h);
   ```

## 树与图的遍历

时间复杂度 `O(n+m)`，`n`表示点数，`m`表示边数。



### 深度优先遍历

```C++
int dfs(int u) {
    st[u] = true; // st[u] 表示点u已经被遍历过
    for (int i = h[u]; i != -1; i = ne[i]) {
        int j = e[i];
        if (!st[j]) dfs(j);
    }
}
```



### 宽度优先遍历

```C++
std::queue<int> q;
st[1] = true; // 表示1号点已经被遍历过
q.push(1);

while (q.size()) {
    int t = q.front();
    q.pop();
    for (int i = h[t]; i != -1; i = ne[i]) {
        int j = e[i];
        if (!st[j]) {
            st[j] = true; // 表示点j已经被遍历过
            q.push(j);
        }
    }
}
```



## 拓扑序模板

时间复杂度`O(n+m)`，`n` 表示点数，`m`表示变边。

```C++
bool topsort() {
    int hh = 0, tt = -1;
    // d[i]存储点i的入度
    for (int i = 1; i <= n; i ++ )
        if (!d[i]) q[ ++ tt] = i;

    while (hh <= tt) {
        int t = q[hh ++ ];
        for (int i = h[t]; i != -1; i = ne[i]) {
            int j = e[i];
            if (-- d[j] == 0)
                q[ ++ tt] = j;
        }
    }

    // 如果所有点都入队了，说明存在拓扑序列；否则不存在拓扑序列。
    return tt == n - 1;
}
```

## dijkstra算法

### 朴素dijkstra算法模板题
时间复杂是 `O(n^2+m)`，`n`表示点数，`m`表示边数.

```C++
int g[N][N];  // 存储每条边
int dist[N];  // 存储1号点到每个点的最短距离
bool st[N];   // 存储每个点的最短路是否已经确定

// 求1号点到n号点的最短路，如果不存在则返回-1
int dijkstra() {
    memset(dist, 0x3f, sizeof dist);
    dist[1] = 0;

    for (int i = 0; i < n - 1; i ++ ) {
        int t = -1;     // 在还未确定最短路的点中，寻找距离最小的点
        for (int j = 1; j <= n; j ++ )
            if (!st[j] && (t == -1 || dist[t] > dist[j]))
                t = j;

        // 用t更新其他点的距离
        for (int j = 1; j <= n; j ++ )
            dist[j] = min(dist[j], dist[t] + g[t][j]);

        st[t] = true;
    }

    if (dist[n] == 0x3f3f3f3f) return -1;
    return dist[n];
}
```



### 堆优化版dijkstra

时间复杂度`O(mlogn)`, `n`表示点数，`m`表示边数

```C++
typedef pair<int, int> PII;

int n;      // 点的数量
int h[N], w[N], e[N], ne[N], idx;       // 邻接表存储所有边
int dist[N];        // 存储所有点到1号点的距离
bool st[N];     // 存储每个点的最短距离是否已确定

// 求1号点到n号点的最短距离，如果不存在，则返回-1
int dijkstra() {
    memset(dist, 0x3f, sizeof dist);
    dist[1] = 0;
    priority_queue<PII, vector<PII>, greater<PII>> heap;
    heap.push({0, 1});      // first存储距离，second存储节点编号

    while (heap.size()) {
        auto t = heap.top();
        heap.pop();

        int ver = t.second, distance = t.first;
        if (st[ver]) continue;
        st[ver] = true;
        for (int i = h[ver]; i != -1; i = ne[i]) {
            int j = e[i];
            if (dist[j] > distance + w[i]) {
                dist[j] = distance + w[i];
                heap.push({dist[j], j});
            }
        }
    }

    if (dist[n] == 0x3f3f3f3f) return -1;
    return dist[n];
}
```

# 基础


##  离散化
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

## 区间合并
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

# 数据结构

## 堆模板
```C++
// h[N]存储堆中的值, h[1]是堆顶，x的左儿子是2x, 右儿子是2x + 1
// ph[k]存储第k个插入的点在堆中的位置
// hp[k]存储堆中下标是k的点是第几个插入的
int h[N], ph[N], hp[N], size;

// 交换两个点，及其映射关系
void heap_swap(int a, int b) {
    swap(ph[hp[a]],ph[hp[b]]);
    swap(hp[a], hp[b]);
    swap(h[a], h[b]);
}

void down(int u) {
    int t = u;
    if (u * 2 <= size && h[u * 2] < h[t]) t = u * 2;
    if (u * 2 + 1 <= size && h[u * 2 + 1] < h[t]) t = u * 2 + 1;
    if (u != t) {
        heap_swap(u, t);
        down(t);
    }
}

void up(int u) {
    while (u / 2 && h[u] < h[u / 2]) {
        heap_swap(u, u / 2);
        u >>= 1;
    }
}

// O(n)建堆
for (int i = n / 2; i; i -- ) down(i);
```

[838. 堆排序](https://www.acwing.com/problem/content/840/0)
```C++
#include <iostream>
#include <vector>

void down(std::vector<int>& heap, int pos, int size) {
    int npos = pos;
    if (2*pos <= size && heap[2*pos] < heap[npos]) npos = 2 * pos;
    if (2*pos+1 <= size && heap[2*pos+1] < heap[npos]) npos = 2 * pos + 1;
    if (npos != pos) {
        std::swap(heap[pos], heap[npos]);
        down(heap, npos, size);
    }
}

int main() {
    int n, m;
    scanf("%d%d", &n, &m);
    
    int size = n;
    std::vector<int> h(n+1);
    for (int i = 1; i <= n; ++i) scanf("%d", &h[i]);
    
    for (int i = n/2; i > 0; --i) down(h, i, size);
    
    while (m--) {
        std::cout << h[1] << " ";
        h[1] = h[size--];
        down(h, 1, size);
    }
    return 0;
}
```


[模拟堆](https://www.acwing.com/problem/content/841/)

# 数学知识



组合数计算，根据加法计数原理有
$$
\begin{array}{C}
C_{a}^{b} &=& C_{a-1}^{b-1} + C_{a-1}^{b}
\end{array}
$$


| 专题   | 题目                                                         | 相关实现                                                     |
| ------ | ------------------------------------------------------------ | ------------------------------------------------------------ |
| 快速幂 | [875. 快速幂](https://www.acwing.com/problem/content/877/)   | [ACW 875快速幂实现](10_QuickPower/875_ACW_E_quick-power.cpp) |
| 组合数 | [885. 求组合数 I]( https://www.acwing.com/problem/content/887/) | [ACW 885. 求组合数 I](10_Math\885_ACW_E_combined-numbers-1.cpp) |
|        |                                                              |                                                              |
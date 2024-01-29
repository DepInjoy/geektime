[ACW算法基础课](https://www.acwing.com/activity/content/introduction/11/)
# 基础算法
## 快速排序
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
[785.快速排序](https://www.acwing.com/problem/content/description/787/)
```C++
// 快排
#include <iostream>

const int N = 100010 + 10;
int data[N];

void quick_sort(int data[], int l, int r) {
    if (l >= r) return;
    
    int x = data[l+r >> 1], i = l - 1, j = r + 1;
    while(i < j) {
        while(data[++i] < x);
        while(data[--j] > x);
        if (i < j) std::swap(data[i], data[j]);
    }
    quick_sort(data, l, j), quick_sort(data, j+1, r);
}

int main() {
    int n;
    scanf("%d", &n);
    for (int i = 0; i < n; ++i) scanf("%d", &data[i]);
    
    quick_sort(data, 0, n-1);
    for (int i = 0; i < n; ++i) printf("%d ", data[i]);
    return 0;
}
```

[786.第k个数](https://www.acwing.com/problem/content/788/)

```C++
#include <iostream>

const int N = 100010;
int data[N];

int quick_select(int data[], int l, int r, int k) {
    if (l == r) return data[l];
    
    int x = data[l + r >> 1], i = l -1, j = r + 1;
    while (i < j) {
        while(data[++i] < x);
        while(data[--j] > x);
        if (i < j) std::swap(data[i], data[j]);
    }
    
    if (j - l + 1 >= k) return quick_select(data, l, j, k);
    return quick_select(data, j + 1, r, k - (j - l + 1));
}

int main() {
    int n, k;
    scanf("%d%d", &n, &k);
    for (int i = 0; i < n; ++i) scanf("%d", &data[i]);
    
    std::cout << quick_select(data, 0, n - 1, k);
}
```

## 归并排序
归并排序模板
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

[787. 归并排序](https://www.acwing.com/problem/content/789/)
```C++
#include <iostream>

const int N = 100010;
int data[N], tmp[N];

void merge_sort(int data[], int l, int r) {
    if (l >= r) return;
    
    int mid = (l + r) >> 1;
    merge_sort(data, l, mid);
    merge_sort(data, mid + 1, r);
    
    int i = l, j = mid + 1, k = 0;
    while (i <= mid && j <= r) {
        if (data[i] <= data[j]) tmp[k++] = data[i++];
        else tmp[k++] = data[j++];
    }
    while(i <= mid) tmp[k++] = data[i++];
    while(j <= r) tmp[k++] = data[j++];
    
    for (int i = 0; i < k; ++i) data[l+i] = tmp[i];
}

int main() {
    int n;
    scanf("%d", &n);
    for (int i = 0; i < n; ++i) scanf("%d", &data[i]);
    
    merge_sort(data, 0, n - 1);
    for (int i = 0; i < n; ++i) printf("%d ", data[i]);
}
```

[AcWing 788. 逆序对的数量](https://www.acwing.com/problem/content/790/)
```C++
#include <iostream>

const int N = 100010;
int data[N], tmp[N];

long long merge_sort(int data[], int l ,int r) {
    if (l >= r) return 0;
    
    int mid = (l + r) >> 1;
    long long ans = merge_sort(data, l, mid) + merge_sort(data, mid + 1, r);
    
    int i = l, j = mid + 1, k = 0;
    while (i <= mid && j <= r) {
        if (data[i] <= data[j]) tmp[k++] = data[i++];
        else {
            tmp[k++] = data[j++];
            ans += mid - i + 1;
        }
    }
    while (i <= mid) tmp[k++] = data[i++];
    while (j <= r) tmp[k++] = data[j++];
    
    for (i = l, j = 0; i <= r; ++i, ++j) data[i] = tmp[j];
    return ans;
}

int main() {
    int n = 0;
    scanf("%d", &n);
    for (int i = 0; i < n; ++i) scanf("%d", &data[i]);
    
    std::cout << merge_sort(data, 0, n-1) << std::endl;
    return 0;
}
```

## 二分
二分实现模板:
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

[789.数的范围](https://www.acwing.com/problem/content/791/)
```C++
#include <iostream>

const int N = 100010;
int data[N];

int main() {
    int n, q;
    scanf("%d%d", &n, &q);
    for (int i = 0; i < n; ++i) scanf("%d", &data[i]);
    
    while (q--) {
        int x;
        scanf("%d", &x);

        int l = 0, r = n - 1;
        while (l < r) {
            int mid = l + r >> 1;
            if (data[mid] >= x) r = mid;
            else l = mid + 1;
        }

        if (data[l] != x) {
            std::cout << "-1 -1" << std::endl;
            continue;
        }
        
        std::cout << l << " ";
        l = 0, r = n -1;
        while (l < r) {
            int mid = l + r + 1 >> 1;
            if (data[mid] <= x) l = mid;
            else r = mid - 1;
        }
        std::cout << l << std::endl;
    }
    return 0;
}
```

[790. 数的三次方根](https://www.acwing.com/problem/content/792/)
```C++
#include <iostream>

int main() {
    double x;
    std::cin >> x;
    
    double l = -10000, r = 10000;
    while (r - l > 1e-8) {
        double mid = (l + r)/2;
        if (mid * mid * mid >= x) r = mid;
        else l = mid;
    }
    printf("%lf", l);
    return 0;
}
```


## 高精度

## 前缀和与差分

## 双指针算法
```C++
or (int i = 0, j = 0; i < n; i ++ )
{
    while (j < i && check(i, j)) j ++ ;

    // 具体问题的逻辑
}
常见问题分类：
    (1) 对于一个序列，用两个指针维护一段区间
    (2) 对于两个序列，维护某种次序，比如归并排序中合并两个有序序列的操作
```

[799.最长连续不重复子序列](https://www.acwing.com/problem/content/801/)

```C++
#include <iostream>

const int N = 100010;
int data[N], cnt[N];

int main() {
    int n;
    scanf("%d", &n);
    for (int i = 0; i < n; ++i) scanf("%d", &data[i]);

    int ans = 0;
    for (int i = 0, j = 0; i < n; ++i) {
        ++cnt[data[i]];
        while (cnt[data[i]] > 1) --cnt[data[j++]];
        ans = std::max(ans, i - j + 1);
    }
    std::cout << ans;
    return 0;
}
```



[800.数组元素的目标和](https://www.acwing.com/problem/content/802/)

```C++
#include <iostream>

const int N = 100010;
int a[N], b[N];

int main() {
    int m, n, x;
    scanf("%d%d%d", &m, &n, &x);
    for (int i = 0; i < m; ++i) scanf("%d", &a[i]);
    for (int i = 0; i < n; ++i) scanf("%d", &b[i]);
    
    for (int i = 0, j = n - 1; i < m; ++i) {
        while(a[i] + b[j] > x) --j;
        if (a[i] + b[j] == x) {
            std::cout << i << " " << j;
        }
    }
    return 0;
}
```

[2816.判断子序列](https://www.acwing.com/problem/content/2818/)

```C++
#include <iostream>

const int N = 100010;
int a[N], b[N];

int main() {
    int m, n;
    scanf("%d%d", &m, &n);
    for (int i = 0; i < m; ++i) scanf("%d", &a[i]);
    for (int i = 0; i < n; ++i) scanf("%d", &b[i]);
    
    int i = 0, j = 0;
    for (; i < n; ++i) {
        if (j < m && a[j] == b[i]) j++;
    }
    std::cout << ((j == m) ? "Yes" : "No");
    return 0;
}
```



## 位运算

```C++
// 求n的第k位数字
n >> k & 1

// 返回n的最后一位1
lowbit(n) = n & -n
```

[801. 二进制中1的个数](https://www.acwing.com/problem/content/803/)
```C++
#include <iostream>

int lowbit(int n) {
   return n & (~n + 1); 
}

int main() {
    int n;
    scanf("%d", &n);
    
    while (n--) {
        int val;
        scanf("%d", &val);
        
        int ans = 0;
        while (val) val -= lowbit(val), ++ans;
        printf("%d ", ans);
    }
    return 0;
}
```

## 离散化

## 区间合并

# 数据结构

## 单链表

## 双链表

## 栈

## 队列



## 单调栈
常见模型：找出每个数左边离它最近的比它大/小的数
```C++
int tt = 0;
for (int i = 1; i <= n; i ++ ) {
    while (tt && check(stk[tt], i)) tt -- ;
    stk[++tt] = i;
}
```

[830.单调栈](https://www.acwing.com/problem/content/832/)

```C++
#include <iostream>

const int N = 100010;
int stk[N], tt;

int main() {
    int n;
    scanf("%d", &n);
    
    while(n--) {
        int x;
        scanf("%d", &x);
        while(tt && stk[tt] >= x) --tt;
        if (tt) {
            std::cout << stk[tt] << " ";
        } else {
            std::cout << "-1 ";
        }
        stk[++tt] = x; 
    }
    return 0;
}
```



## 单调队列
常见模型：找出滑动窗口中的最大值/最小值
```C++
int hh = 0, tt = -1;
for (int i = 0; i < n; i ++ ) {
    while (hh <= tt && check_out(q[hh])) hh ++ ;  // 判断队头是否滑出窗口
    while (hh <= tt && check(q[tt], i)) tt -- ;
    q[ ++ tt] = i;
}
```
[154.滑动窗口](https://www.acwing.com/problem/content/156/)

```C++
#include <iostream>

const int N = 1000010;
int q[N], d[N];

int main() {
    int n, k;
    scanf("%d%d", &n, &k);
    for (int i = 0; i < n; ++i) scanf("%d", &d[i]);
    
    int hh = 0, tt = -1;
    for (int i = 0; i < n; ++i) {
        if (hh <= tt && q[hh] < i - k + 1) ++hh;
        while (hh <= tt && d[q[tt]] >= d[i]) --tt;
        q[++tt] = i;
        if (i >=  k - 1) std::cout << d[q[hh]] << " ";
    }
    std::cout << std::endl;
    
    hh = 0, tt = -1;
    for (int i = 0; i < n; ++i) {
        if (hh <= tt && q[hh] < i - k + 1) ++hh;
        while(hh <= tt && d[q[tt]] <= d[i]) --tt;
        q[++tt] = i;
        if (i >= k - 1) std::cout << d[q[hh]] << " ";
    }
    return 0;
}
```

## KMP
实现模板
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
[831. KMP字符串](https://www.acwing.com/problem/content/833/)
> 给定一个字符串S,以及一个模式串P所有字符串中只包含大小写英文字母以及阿拉伯数字。
> 模式串 P在字符串 S中多次作为子串出现。
> 求出模式串P在字符串 S中所有出现的位置的起始下标。

```C++

```

## Trie

```C++
int son[N][26], cnt[N], idx;
// 0号点既是根节点，又是空节点
// son[][]存储树中每个节点的子节点
// cnt[]存储以每个节点结尾的单词数量

// 插入一个字符串
void insert(char *str) {
    int p = 0;
    for (int i = 0; str[i]; i ++ ) {
        int u = str[i] - 'a';
        if (!son[p][u]) son[p][u] = ++ idx;
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

[835.Trie字符串统计](https://www.acwing.com/problem/content/837/)

```C++
```



[143.最大异或对](https://www.acwing.com/problem/content/145/)

```C++
```


## 并查集

## 堆
[838. 堆排序](https://www.acwing.com/problem/content/840/)
```C++
#include <iostream>

const int N = 100010;
int h[N], size;

void down(int i) {
    int t = i;
    if (2*i <= size && h[2*i] < h[t]) t = 2 * i;
    if (2*i+1 <= size && h[2*i+1] < h[t]) t = 2 * i + 1;
    if (t != i) {
        std::swap(h[t], h[i]);
        down(t);
    }
}

int main() {
    int m;
    scanf("%d%d", &size, &m);
    for(int i = 1; i <= size; ++i) scanf("%d", &h[i]);
    
    // 自底向上建堆O(N)
    for (int i = size/2; i; --i) down(i);

    while(m--) {
        printf("%d ", h[1]);
        h[1] = h[size--];
        down(1);
    }
    return 0;
}
```

[模拟堆](https://www.acwing.com/problem/content/841/)
```C++

```
## 哈希表

# 动态规划

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
6 = 1 = 2 + 3

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



# 搜索和图论

## DFS
[842. 排列数字](https://www.acwing.com/problem/content/844/)
> 给定一个整数 n，将数字 1∼n 排成一排，将会有很多种排列方法。现在，请你按照字典序将所有的排列方法输出

```C++
#include <vector>
#include <iostream>

void dfs(int pos, const int n, std::vector<int> &path,
        std::vector<int>& st) {
    if (pos == n) {
        for (int i = 0 ; i < n; ++i) {
            std::cout << path[i] << " ";
        }
        std::cout << std::endl;
        return;
    }
    
    for (int i = 1; i <= n; ++i) {
        if (!st[i]) {
            path[pos] = i;
            st[i] = true;
            dfs(pos + 1, n, path, st);
            st[i] = false;
        }
    }
}

int main() {
    int n;
    scanf("%d", &n);
    
    std::vector<int> path(n), st(n+1);
    dfs(0, n, path, st);
    return 0;
}
```

[843. n-皇后问题](https://www.acwing.com/problem/content/845/)

[n-皇后问题(按行枚举或按每个元素枚举)](https://www.acwing.com/solution/content/2820/) 这个解析不错



逐行尝试

```C++
#include <vector>
#include <iostream>

void dfs(const int x, const int n, std::vector<std::vector<char>>& q,
        std::vector<bool>& col, std::vector<bool>& dg, std::vector<bool>& udg) {
    // 已经搜索了n行,输出
    if (x == n) {
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < n; ++j) {
                std::cout << q[i][j];
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
        return;
    }
    
    for (int y = 0; y < n; ++y) {
        // 剪枝(列, 对角线和反对角线都没有放皇后)放皇后尝试下一行
        if (!col[y] && !dg[x + y] && !udg[n - x + y]) {
            q[x][y] = 'Q';
            col[y] = dg[x + y] = udg[n - x + y] = true;
            dfs(x+1, n, q, col, dg, udg);
            // 恢复现场
            q[x][y] = '.';
            col[y] = dg[x + y] = udg[n - x + y] = false;
        }
    }
}

int main() {
    int n;
    scanf("%d", &n);
    
    std::vector<std::vector<char>> q(n, std::vector<char>(n, '.'));
    // 对角线和反对角线是否放皇后
    std::vector<bool> dg(2*n), udg(2*n);
    // 列是否放皇后
    std::vector<bool> col(n);
    dfs(0, n, q, col, dg, udg);
    return 0;
}
```
逐个尝试法

```C++
#include <vector>
#include <iostream>

void dfs(int x, int y, int cnt, const int n, std::vector<std::vector<char>>& q,
            std::vector<bool>& row, std::vector<bool>& col,
            std::vector<bool>& dg, std::vector<bool>& udg) {
    if (y == n) y = 0, ++x;
    // 放了超过n个皇后
    if (cnt > n) return;
    // 最后一行且恰好放了n个皇后,才是有效结果
    if (x == n) {
        if (cnt == n) {
            for (int i = 0; i < n; ++i) {
                for (int j = 0; j < n; ++j) {
                    std::cout << q[i][j];
                }
                std::cout << std::endl;
            }
            std::cout << std::endl;
        }
        return;
    }


    // 分支1:[x][y]放皇后
    if (!row[x] && !col[y] && !dg[x + y] && !udg[n - x + y]) {
        q[x][y] = 'Q';
        row[x] = col[y] = dg[x + y] = udg[n - x + y] = true;
        dfs(x, y+1, cnt + 1, n, q, row, col, dg, udg);
        q[x][y] = '.';
        row[x] = col[y] = dg[x + y] = udg[n - x + y] = false;
    }

    // 分支2: [x][y]不放皇后
    dfs(x, y+1, cnt, n, q, row, col, dg, udg);
}

int main() {
    int n;
    scanf("%d", &n);
    
    std::vector<std::vector<char>> q(n, std::vector<char>(n, '.'));
    // 反对角线和对角线上是否存在皇后
    std::vector<bool> dg(2*n), udg(2*n);
    // 行和列上是否存在皇后
    std::vector<bool> col(n), row(n);
    dfs(0, 0, 0, n, q, row, col, dg, udg);
    return 0;
}
```

## BFS
[844. 走迷宫](acwing.com/problem/content/846/)

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

## 堆
代码模板
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
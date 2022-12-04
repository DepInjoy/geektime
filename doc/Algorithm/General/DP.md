
# 动态规划
在采用动态规划时总是用递归的思路分析问题，即把大问题分解成小问题，再把小问题的解合起来形成大问题的解。找出描述大问题的解和小问题的解之间递归关系的状态转移方程是采用动态规划解决问题的关键所在。

动态规划的题目分为两大类
- 一种是求最优解类，典型问题是背包问题，此类问题的地推性质具有一个名字-最优子结构，即当前问题的最优解取决于子问题的最优解。
- 另一种是计数类，比如[LeetCode 63.不同路径 II](https://leetcode.cn/problems/unique-paths-ii/)统计方案数的问题，它们都存在一定的递推性质。当前问题的方案数取决于子问题的方案数。

## 斐波那契数列问题
> [LeetCode 509](https://leetcode-cn.com/problems/fibonacci-number)：斐波那契数 （通常用 F(n) 表示）形成的序列称为斐波那契数列 。该数列由 0 和 1 开始，后面的每一项数字都是前面两项数字的和。也就是：
> F(0) = 0，F(1) = 1
> F(n) = F(n - 1) + F(n - 2)，其中 n > 1
> 给定 n ，请计算 F(n)
该问题可以通过以下种方式来实现：
- 1. 递归实现，时间复杂度为$O(2^N)$
```C++
int fib(int n) {
    if (n <= 1) return n;
    if (n == 2) return 1;
    return fib(n-1)+fib(n-2);
}
```
我们在计算$F(N)$的时候前面已经计算过$F(n-1)$可以通过记忆$F(n-1)$来降低计算的复杂度。

- 2. 动态规划。时间复杂度为$O(N)$,空间复杂度为$O(N)$
```C++
int fib(int n) {    
    vector<int> dp = vector<int>(n+2);
    dp[0] = 0, dp[1] = 1;
    for (int i = 2; i <= n; i++) {
        dp[i] = dp[i-1] + dp[i-2];
    }
    return dp[n];
}
```
$f[n]$只依赖$f[n-1]$和$f[n-2]$可以进行空间压缩，来降低空间复杂度。

- 3. 动态规划+空间压缩,时间复杂度为$O(N)$,空间复杂度为$O(1)$
```C++
int fib(int n) {
    if (n <= 1) return n;

    int pre = 1, prepre = 0;
    int cur = 1;
    for (int i = 2; i <= n; i++) {
        cur = pre + prepre;
        prepre = pre;
        pre = cur;
    }
    return cur;
}
```
- 4. 矩阵快速幂,时间复杂度为$O(log_{2}N)$,空间复杂度为$O(1)$
$$
\begin{array}{l}
\begin{bmatrix}
  F(N)\\
  F(N-1)
\end{bmatrix}  = \begin{bmatrix}
F(N-1) + F(N-2) \\
F(N-1)
\end{bmatrix} = \begin{bmatrix}
 1 & 1\\
 1 & 0
\end{bmatrix} \begin{bmatrix}
F(N-1) \\
F(N-2)
\end{bmatrix}
\\
\\
可以推出：\\
\begin{bmatrix}
  F(N)\\
  F(N-1)
\end{bmatrix} = \begin{bmatrix}
  1 & 1\\
  1 & 0
\end{bmatrix}^{N-1} \begin{bmatrix}
 F(1)\\
 F(0)
\end{bmatrix}
\end{array}
$$
快速幂算法可以快速计算$M^N$在$O(log_{2}N)$的时间复杂度内计算得到结果。
快速幂算法源码较长，可以参考[实现Github源码](https://github.com/DepInjoy/geektime/blob/main/algorithm/CPP/topic/DP/00_FibonacciNumber/L509_FibonacciNumber.cpp)

- 5. 通项公式
$$
\begin{array}{l}
斐波那契数F(n)是齐次线性递推，根据递推方程F(N) = F(N-1) + F(N-2),可得到特征方程\\
x^2=x+1
\\
求得x_1=\frac{1+\sqrt{5}}{2},x_1=\frac{1-\sqrt{5}}{2},设通解为 F(n)=c_1x_1^n+c_2x_2^n\\
初始值F(0)=0,F(1)=1，代入计算得到c_1=\frac{1}{\sqrt{5}},c_2=-\frac{1}{\sqrt{5}}\\
由此得：F(n)=\frac{1}{\sqrt{5}} \left[\left( \frac{1+\sqrt{5}}{2}\right)^n - \left( \frac{1-\sqrt{5}}{2}\right)^n \right ]
\end{array}
$$

```c++
int fib(int n) {
    double sqrt5 = sqrt(5);
    double fibN = pow((1 + sqrt5) / 2, n) - pow((1 - sqrt5) / 2, n);
    return round(fibN / sqrt5);
}
```
### 类似扩展
- 1. **爬楼梯问题**
> [LeetCode 70：爬楼梯问题](https://leetcode-cn.com/problems/climbing-stairs/) 设你正在爬楼梯。需要 n 阶你才能到达楼顶。每次你可以爬 1 或 2 个台阶。
这就是一个斐波那契额数列问题，状态转移方程：$F(n)=F(n-1)+F(n-2)$

[实现github源码](https://github.com/DepInjoy/geektime/blob/main/algorithm/CPP/topic/DP/00_FibonacciNumber/L70_ClimbStairs.cpp)

- 2. 第N个泰波那契数
> [LeetCode1137 第 N 个泰波那契数](https://leetcode-cn.com/problems/n-th-tribonacci-number) 泰波那契序列 Tn 定义如下： T0=0, T1=1, T2=1, 且在n>= 0 的条件下Tn+3=Tn+Tn+1+Tn+2。给你整数n，请返回第n个泰波那契数Tn的值

快速幂算法递归公式
$$
\begin{array}{l}
\begin{bmatrix}
 T_n\\
 T_{n-1}\\
 T_{n-2}
\end{bmatrix} = \begin{bmatrix}
 T_{n-3} + T_{n-2} + T_{n-1}\\
 T_{n-1}\\
 T_{n-2}
\end{bmatrix} = \begin{bmatrix}
1 & 1 & 1 \\
1 & 0 & 0 \\
0 & 1 & 0
\end{bmatrix} \begin{bmatrix}
T_{n-1} \\
T_{n-2} \\
T_{n-3}
\end{bmatrix}
\\
\\
从而，可以推出：\\
\begin{bmatrix}
 T_n\\
 T_{n-1}\\
 T_{n-2}
\end{bmatrix} = \left (\begin{bmatrix}
1 & 1 & 1 \\
1 & 0 & 0 \\
0 & 1 & 0
\end{bmatrix} \right )^{n-2}\begin{bmatrix}
 T_2\\
 T_{1}\\
 T_{0}
\end{bmatrix} \\
其中：T_0=0, T_1=1, T_2=1
\end{array} 
$$
[动态规划和快速幂等实现github源码](https://github.com/DepInjoy/geektime/blob/main/algorithm/CPP/topic/DP/00_FibonacciNumber/L1137_tribonacci.cpp)

## 背包问题
背包问题是一类经典的可以应用动态规划来解决的问题。背包问题的基本描述如下：给定一组物品，每种物品都有其重量和价格，在限定的总重量内如何选择才能使物品的总价格最高。由于问题是关于如何选择最合适的物品放置于给定的背包中，因此这类问题通常被称为背包问题。根据物品的特点，背包问题还可以进一步细分。
- 如果每种物品只有一个，可以选择将之放入或不放入背包，那么可以将这类问题称为0-1背包问题。0-1背包问题是最基本的背包问题，其他背包问题通常可以转化为0-1背包问题。
- 如果第i种物品最多有$M_i$个，也就是每种物品的数量都是有限的，那么这类背包问题称为有界背包问题（也可以称为多重背包问题）。
- 如果每种物品的数量都是无限的，那么这类背包问题称为无界背包问题（也可以称为完全背包问题）[1]
### 0-1背包问题
> 有 N 件物品和一个容量是 V 的背包。每件物品只能使用一次.第 i 件物品的体积是 vi，价值是 wi。求解将哪些物品装入背包,可使这些物品的总体积不超过背包容量，且总价值最大。输出最大价值
$$
\begin{array}{l}
状态转移方程:\\
dp[i][j] = max(dp[i-1][j], dp[i-1][j-v[i]] + w[i])
\\
\\
对于j的遍历，从大到小进行更新，可以进行空间压缩到一维dp[j] \\
dp[j] = max(dp[j], dp[j-v[i]] + w[i])
\end{array}
$$
```C++
// n为物品数量, m为背包体积
for (int i = 0; i < n; i++) {
    for (int j = m; j >= 0 && j - v[i] >= 0; j--) {
        dp[j] = max(dp[j], dp[j - v[i]] + w[i]);
    }
}
//最终结果为dp[m];
```

### 完全背包问题
> 有 N 种物品和一个容量是 V 的背包，每种物品都有无限件可用。第 i 种物品的体积是 vi，价值是 wi。求解将哪些物品装入背包,可使这些物品的总体积不超过背包容量，且总价值最大。输出最大价值。

$$
\begin{array}{l}
状态转移方程:\\
第i个物品可被选择的数量为0, 1, 2 ......, k ...\\
dp[i][j] = max(dp[i-1][j], \\
dp[i-1][j-v[i]]+w[i], dp[i-1][j-2*v[i]] + 2*w[i], ....., dp[i-1][j-k*v[i]] + k*w[i], ......)\\
\\
dp[i-1][j-v[i]] = max(\\
dp[i-1][j-v[i]], dp[i-1][j-2*v[i]]+w[i], dp[i-1][j-3*v[i]] + 2*w[i], ....., dp[i-1][j-(k+1)*v[i]] + k*w[i], ......)\\
\\
\\
可以得出：\\
dp[i][j] = max(dp[i-1][j], dp[i-1][j-v[i]] + w[i])
\\
\\
和01背包类似，将j从0到m遍历，可以进行空间压缩\\
dp[j] = max(dp[j], dp[j-v[i]]+w[i])
\end{array}
$$
```c++
// n为物品数量, m为背包体积
for (int i = 0; i < n; i++) {
    for (int j = v[i]; j <= m; j++) {
        dp[j] = max(dp[j], dp[j - v[i]] + w[i]);
    }
}
// 最终结果为dp[m];
```
### 多重背包问题
> 有 N 种物品和一个容量是 V 的背包。第 i 种物品最多有 si 件，每件体积是 vi，价值是 wi。求解将哪些物品装入背包，可使物品体积总和不超过背包容量，且价值总和最大。
$$
\begin{array}{l}
dp[i][j] = max(dp[i-1][j], dp[i-1][j-v_i]+w[i], dp[i-1][j-2*v_i] + 2*w[i], ....., dp[i-1][j-k*v_i] + k*w[i]) \\
其中，k <= s[i]且j - k * v_i >= 0
\end{array}
$$
#### 二进制优化
将这$s_i$件物品拆成系数构成一组价值和体积和系数乘积的物品，便可以将高问题转化为01背包问题。这些系数分别为$1, 2, 4, ......, 2^{k-1}, n - 2^{k}+1$,其中，k是满足$n-2^k+1$的最大正整数。

例如，13可以拆解为$1, 2, 4, 6$。

```c++
#include <iostream>
#include <vector>
using namespace std;

const int N = 12010, M= 2010;
int v[N], w[N];
int dp[M];

int main() {
    int n, m;
    cin >> n >> m;
    int cnt = 0;
    for (int i = 0; i < n; i++) {
        int vi, wi, si;
        cin >> vi >> wi >> si;
        // 系数拆解
        for (int k = 1; k <= si; k *= 2) {
            v[++cnt] = vi * k;
            w[cnt] = wi * k;
            si -= k;
        }
        if (si > 0) {
            v[++cnt] = vi * si;
            w[cnt] = wi * si;
        }
    }

    // 转化为01背包问题
    n = cnt;
    for (int i = 1; i <= n; i++) {
        for (int j = m; j >= v[i]; j--) {
            dp[j] = max(dp[j], dp[j - v[i]] + w[i]);
        }
    }
    cout << dp[m];
    return 0;
}
```
#### 单调队列优化

```C++
#include <iostream>
#include<cstring>

using namespace std;

const int N = 20010;

int n, m;
int f[N], g[N], q[N];

int main() {
    cin >> n >> m;
    for (int i = 0; i < n; i++) {
        int v, w, s;
        cin >> v >> w >> s;
        memcpy(g, f, sizeof(f));
        
        for (int j = 0; j < v; j++) {
            int hh = 0, tt = -1;
            for (int k = j; k <= m; k += v) { // k表示m%v的第几个数
                f[k] = g[k];
                if (hh <= tt && k-s*v > q[hh]) hh++;
                if (hh <= tt) f[k] = max(f[k], g[q[hh]]+(k-q[hh])/v*w);
                while(hh <= tt && g[q[tt]]-(q[tt]-j)/v*w <= g[k]-(k-j)/v*w) tt--;
                q[++tt] = k;
            }
        }
    }

    cout << f[m] << endl;
    return 0;
}
```
### 分组背包问题

### 练习题
- [剑指 Offer II 101. 分割等和子集](https://leetcode.cn/problems/NUPfPr/)
- [剑指 Offer II 102. 加减的目标值](https://leetcode.cn/problems/YaVDxD/)
- [剑指 Offer II 103. 最少的硬币数目](https://leetcode.cn/problems/gaM7Ch/)

## 矩阵最小路径和
> 给定一个包含非负整数的mxn网格grid,请找出一条从左上角到右下角的路径，使得路径上的数字总和为最小。[LeetCode链接](https://leetcode.cn/problems/minimum-path-sum/description/)

对于第一行的第i个元素只能从左边走到右边，因此有：
$$
dp[0][0] = grid[0][0] \\
dp[0][i] = dp[0][i-1] + grid[0][i]
$$
同样，对于第一列的第i个元素只能从其上边走到当前位置，因此有
$$
dp[i][0] = dp[i-1][0] + grid[i][0]
$$
对于$(i, j)$可以从上边或左边的位置走到当前位置，寻找最短路径，因此有:
$$
dp[i][j] = min(dp[i-1][j], dp[i][j-1]) + grid[i][j]
$$

C++实现代码如下
```C++
int minPathSum(std::vector<std::vector<int>>& grid) {
    if (grid.size() == 0 || grid[0].size() == 0) {
        return 0;
    }

    int m = grid.size(), n = grid[0].size();
    std::vector<std::vector<int>> dp(m, std::vector<int>(n));
    dp[0][0] = grid[0][0];
    for (int i = 1; i < m; ++i) {
        dp[i][0] = dp[i-1][0] + grid[i][0];
    }
    for (int i = 1; i < n; ++i) {
        dp[0][i] = dp[0][i-1] + grid[0][i]; 
    }

    for (int i = 1; i < m; ++i) {
        for (int j = 1; j < n; ++j) {
            dp[i][j] = std::min(dp[i-1][j], dp[i][j-1]) + grid[i][j];
        }
    }
    return dp[m-1][n-1];
}
```
实现的时间和空间复杂度为$O(M*N)$

对于(i, j)位置在实际执行时只关心正上方和左方位置的值
```
           x   x   x   x
           x   x   .   x
           x   .   o   x
           x   x   x   x
```
因此，可以进行空间压缩，采用$min(M, N)$的来存储动态规划的过程信息，假设行数(M)较小，那么按列执行动态规划，其中$dp[i]$代表的便是到达左边位置的最小路径和，$dp[i-1]$代表的是到达正上方位置所需要的最小路径和，因此可以有如下的实现:
```C++
// 动态规划+空间压缩
// 时间复杂度为O(MxN),空间复杂度为O(Min(M,N))
int minPathSum(vector<vector<int>>& grid) {
    if (grid.size() == 0 || grid[0].size() == 0) {
        return 0;
    }

    int m = grid[0].size(), n = grid.size();
    bool isSmallRow = (m <= n); // 行数<列数为true,否则false
    // 行列中较
    int N = (isSmallRow ? m : n);
    std::vector<int> dp(N);
    dp[0] = grid[0][0];
    for (int i = 1; i < N; ++i) {
        dp[i] = dp[i-1] + (isSmallRow ? grid[0][i] : grid[i][0]);
    }

    int M = (isSmallRow ? n : m); // 行列中较大者
    for (int i = 1; i < M; ++i) {
        dp[0] = dp[0] + (isSmallRow ? grid[i][0] : grid[0][i]);
        for (int j = 1; j < N; ++j) {
            dp[j] = std::min(dp[j-1], dp[j]) + (isSmallRow ? grid[i][j] : grid[j][i]);
        }
    }
    return dp[N-1];
}
```
## 矩阵路径问题
> 矩阵路径是一类常见的可以用动态规划来解决的问题。这类问题通常输入的是一个二维的格子，一个机器人按照一定的规则从格子的某个位置走到另一个位置，要求计算路径的条数或找出最优路径。
> 
> 矩阵路径相关问题的状态方程通常有两个参数，即f(i,j)是机器人当前的位置，需要根据路径的特点找出到达坐标(i，j)之前的位置，通常是坐标(i-1，j-1), (i-1, j), (i, j-1)中的一个或多个。相应地，状态转移方程就是找出f(i,j)与f(i-1，j-1), f(i-1, j), f(i, j-1)之间的关系。可以根据状态转移方程写出递归代码，但值得注意的是一定要将f(i, j)的计算结果用一个二维数组缓存，以避免不必要的重复计算。也可以将计算所有f(i, j)看成填充二维表格的过程，相应地，可以创建一个二维数组并逐一计算每个元素的值。
> 
> **通常，矩阵路径相关问题的代码都可以优化空间效率，用一个一维数组就能保存所有必需的数据**[1]。

### 路径的数目
[LeetCode 62 不同路径](https://leetcode.cn/problems/unique-paths)
$$
\begin{array}{l}
f[i][j]=f[i-1][j]+f[i][j-1]
\end{array}
$$
滚动数组实现
```c
int uniquePaths(int m, int n) {
    vector<int> dp(n, 1);
    for (int i = 1; i < m; i++) {
        for (int j = 1; j < n; j++) {
            dp[j] += dp[j-1];
        }
    }
    return dp[n-1];
}
```
[LeetCode 63 不同路径 II](https://leetcode.cn/problems/unique-paths-ii)此问题会有路障
$$
\begin{array}{l}
状态初始化: f[0][0] = (matrix[0][0] == 0)\\
\\
状态转移方程： \\
f[i][j] = \begin{cases}
  & 0 & matrix[i][j] = 0 \\
  & f[i-1][j]+f[i][j-1] & matrix[i][j] \ne 0
\end{cases}
\end{array}
$$

## 双序列问题

## 单序列问题

## 记忆搜索

- [516.最长回文子序列](https://leetcode-cn.com/problems/longest-palindromic-subsequence/)
- [329. 矩阵中的最长递增路径](https://leetcode-cn.com/problems/longest-increasing-path-in-a-matrix/)
- [51. N 皇后](https://leetcode-cn.com/problems/n-queens/)
- [52. N皇后 II](https://leetcode-cn.com/problems/n-queens-ii/)

# 练习题
- [LeetCode 42. 接雨水(H)](https://leetcode-cn.com/problems/trapping-rain-water/)

# 参考资料
- 1 《剑指Offer(专项突破版)：数据结构与算法名企面试题精讲》
- 2 《》

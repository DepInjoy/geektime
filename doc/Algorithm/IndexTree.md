树状数组(Binary Indexed Trees)，可多用于高效计算数列的前缀和以及区间和。对于长度为n的数组，它可以以$O(logn)$的时间得到任意前缀和$\sum_{i=1}^{j}a[i], 1\le j \le N$，并同时支持在  $O(logn)$时间内支持动态单点值的修改，空间复杂度$O(n)$。

# 算法原理
假设输入为数组`arr`要对其实现前缀和和区间求和。
$$
\begin{array}{l}
​BIT[i] = \sum_{i−lowestbit(i)+1}^{i} arr[k]\\
\\
其中，lowestbit(i)指将i转为二进制后，最后一个1的位置所代表的数值
\end{array}
$$



下图是初始化后的情况，横轴为数组的下标(记为i)，纵轴为下标数值对应的`lowestbit(i&-i)`，长方形表示`BIT[i]`涵盖的求和的范围

![](./img/IndexTree.jpg)

对于每个节点都有以下特性：

- 假如`i`是左子节点，那么其父节点下标为`i+(lowestbit(i))`
- 假如`i`是右子节点，那么其父节点下标为`i-(lowestbit(i))`



## 数据操作

### 更新一个值

如果修改原始数组`arr`中下标为`i`的值，由于BIT数组涵盖了`arr[i]`的值以及下标为`i`以及所有父节点，实现伪码为

```C++
while i < n:
    BIT[i] += new_value
    i += (i&-i)
```

### 区间求和

如果要求`arr`数组下标区间为`[i, j]`的数值之和，那么可以先求下标为`[0,i-1]`的数值之和，再求下标为`[0,j]`的数值之和，然后用后者减去前者。求[0, i]可以通过下面的方法，实现伪码：

```C++
sum = 0
while i > 0:
    sum += BIT[i]
    i -= (i&-i)
```



# 练习题

- [307. 区域和检索 - 数组可修改](https://leetcode-cn.com/problems/range-sum-query-mutable/)
- [308. 二维区域和检索 - 可变](https://leetcode-cn.com/problems/range-sum-query-2d-mutable/)

# 参考资料

1. [Binary Indexed Trees 简介](https://wulc.me/2016/07/12/Binary%20Indexed%20Trees%20%E7%AE%80%E4%BB%8B/)


## 一维前缀和

## 二维前缀和
设二维数组A的大小为m*n，行下标的范围为[1, m]，列下标的范围为[1, n]。
数组P是A的前缀和数组，等价于P中的每个元素P[i][j]：
- 如果i和j均大于 0，那么 P[i][j]表示 A 中以(1, 1)为左上角，(i, j)为右下角的矩形区域的元素之和；
- 如果i和j中至少有一个等于 0，那么P[i][j] 也等于 0。
数组P可以帮助我们在 O(1)的时间内求出任意一个矩形区域的元素之和。具体地，设我们需要求和的矩形区域的左上角为 (x1, y1)，右下角为 (x2, y2)，则该矩形区域的元素之和可以表示为：
$$
\begin{array}{l}
sum \\ 
= A[x1..x2][y1..y2] \\
= P[x2][y2]-P[x1-1][y2]-P[x2][y1-1]+P[x1-1][y1-1]
\end{array}
$$
那么，如何得到P
$$
\begin{array}{l}
考虑 (i, j) 这个 1 * 1 的矩形区域，根据上面的等式，有：\\
\\
A[i][j] = P[i][j] - P[i - 1][j] - P[i][j - 1] + P[i - 1][j - 1]
\end{array}
$$


# 参考资料
- 1 [1292. 元素和小于等于阈值的正方形的最大边长官方题解](https://leetcode.cn/problems/maximum-side-length-of-a-square-with-sum-less-than-or-equal-to-threshold/solution/yuan-su-he-xiao-yu-deng-yu-yu-zhi-de-zheng-fang-2/)
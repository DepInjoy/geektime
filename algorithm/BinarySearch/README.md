
# 二查查找
- [LeetCode-704:二分查找](https://leetcode-cn.com/problems/binary-search/)
在这个实现中获得的比较大的收获在于，取middle的时候，采取`left + (right - left) / 2`的方式，可以避免left和right都很大的情况下的一些溢出问题，实现采用移位方式更便于计算机计算。
```C++
int middle = left + ((right - left) >> 1);
```
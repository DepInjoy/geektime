# 栈

## 单调栈
经典类型：找出每个数左边离它最近的比它大/小的数
```C++
int tt = 0;
for (int i = 1; i <= n; i ++) {
    while (tt && check(stk[tt], i)) tt --;
    stk[ ++ tt] = i;
}
```

- [LeetCode 42. 接雨水](https://leetcode-cn.com/problems/trapping-rain-water/)
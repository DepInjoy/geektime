# 队列

## 单调队列
经典类型：找出滑动窗口中的最大值/最小值

如果采用数组模拟队列
```C++
int hh = 0, tt = -1;
for (int i = 0; i < n; i ++) {
    while (hh <= tt && check_out(q[hh])) hh ++ ;  // 判断队头是否滑出窗口
    while (hh <= tt && check(q[tt], i)) tt -- ;
    q[ ++ tt] = i;
}
```

直接采用STL的`std::deque()`
```C++
for (int i = 0; i < nums.size(); i++) {
    if (!q.empty() && i - q.front() + 1 > k) q.pop_front();
    while(!q.empty() && check(q.back(), i)) q.pop_back();
    q.push_back(i);
}
```

### 相关练习题
- [239. 滑动窗口最大值](https://leetcode.cn/problems/sliding-window-maximum/)
- [剑指 Offer59-II.队列的最大值](https://leetcode.cn/problems/dui-lie-de-zui-da-zhi-lcof)
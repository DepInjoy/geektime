# 二分查找

二分查找需要输入数据需要满足如下条件：

- 单调递增或者递减(Sorted)
- 存在上下界(Bounded)
- 能够通过索引访问(Accessible by index)

算法实现复杂度为$O(logN)$



## 整数二分查找

最经典的二分查找，在一个有序数组中，找某个数是否存在或所在位置索引

```java
while (L <= R) {
    int mid =  L + ((R - L) >> 1);
    if (sortedArr[mid] < num) {
        L = mid + 1;
    } else if (sortedArr[mid] > num) {
        R = mid - 1;
    } else {
        // 找到目标值，返回相应信息
        return true // 或者mid ;
    }
}
```



在一个有序数组中，找>=某个数最左侧的位置

```java
int ans = -1;
while (L <= R) {
    int mid = L + ((R - L) >> 1);
    if (sortedArr[mid] >= value) {
        ans = mid;
        R = mid - 1;
    } else {
        L = mid + 1;
    }
}
```



在一个有序数组中，找<=某个数最右侧的位置

```java
int ans = -1;
while (L <= R) {
    int mid = L + ((R - L) >> 1);
    if (arr[mid] <= value) {
        ans = mid;
        L = mid + 1;
    } else {
        R = mid - 1;
    }
}
```



## 练习题

1. [sqrtx](https://leetcode.com/problems/sqrtx)
2. [valid-perfect-square](https://leetcode.com/problems/valid-perfect-square/)


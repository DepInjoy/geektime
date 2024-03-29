# 动态规划
1. [2. 01背包问题](https://www.acwing.com/problem/content/2/) [相关实现](02_DP/02_ACW_E_01-bag.cpp) 01背包问题
2. [3. 完全背包问题](https://www.acwing.com/problem/content/3/) [相关实现](02_DP/03_ACW_E_full-bag.cpp) 完全背包问题

---

1. [213. 打家劫舍 II](https://leetcode-cn.com/problems/house-robber-ii/)                         动态规划   [相关实现](02_DP/213_LC_M_house-robber-ii.cpp)
2. [122. 买卖股票的最佳时机 II](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-ii/description)    动态规划+空间压缩 [相关实现](02_DP\122_LC_M_best-time-to-Buy-And-Sell-Stock-ii.cpp)
3. [416. 分割等和子集](https://leetcode.cn/problems/partition-equal-subset-sum/description/) [相关实现](02_DP/416_LC_M_partition-equal-subset-sum.cpp)01背包问题
4. [518. 零钱兑换 II](https://leetcode.cn/problems/coin-change-ii/description/) [相关实现](02_DP/518_LC_M_coin-change-ii.cpp) 完全背包(组合数问题)

5. [72. 编辑距离](https://leetcode.cn/problems/edit-distance/) [相关实现](02_DP\72_LC_M_edit-distance.cpp) 线性DP
6. [377. 组合总和 Ⅳ](https://leetcode.cn/problems/combination-sum-iv/description/) [相关实现]()     动态规划

# 贪心
1. [905. 区间选点](https://www.acwing.com/problem/content/description/907/) [相关实现](06_Greedy/950_ACW_M_interval-point-selection.cpp) 贪心

# 回溯

| 专题 | 题目                                                    | 相关实现                                                   | 其他 |
| ---- | ------------------------------------------------------- | ---------------------------------------------------------- | ---- |
| 回溯 | [47.全排列II](03_BackTrack/47_LC_M_permutations-ii.cpp) | [LC 47.全排列II](03_BackTrack/47_LC_M_permutations-ii.cpp) |      |
|      |                                                         |                                                            |      |
|      |                                                         |                                                            |      |



# 堆

1. [347. 前 K 个高频元素](https://leetcode.cn/problems/top-k-frequent-elements/description) [相关实现](04_Heap/347_LC_M_top-k-frequent-elements.cpp)  堆/快排+ Hash表
2. [692. 前K个高频单词](https://leetcode.cn/problems/top-k-frequent-words/description) [相关实现](./04_Heap/692_LC_M_top-k-frequent-words.cpp) 堆/快排+Hash表
3. [703. 数据流中的第 K 大元素](https://leetcode.cn/problems/kth-largest-element-in-a-stream/description/) [相关实现](04_Heap/703_LC_E_kth-largest-element-in-a-stream.cpp)

100 [838. 堆排序-ACW](https://www.acwing.com/problem/content/840/) [相关实现](04_Heap/838_ACW_E_heap-sort.cpp) 手写堆(优化建堆时间复杂度为O)/堆

# 单调栈
1. [496. 下一个更大元素 I](https://leetcode.cn/problems/next-greater-element-i/description/)       [相关实现](02_MonotonicStack/496_E_Next-Greater-Element-i.cpp)      单调栈

# 双指针

| 专题   | 题目                                                         | 实现                                                         | 其他   |
| ------ | ------------------------------------------------------------ | ------------------------------------------------------------ | ------ |
| 双指针 | [977. 有序数组的平方](https://leetcode.cn/problems/squares-of-a-sorted-array/description/) | [LeetCode 977. 有序数组的平方](01_DoublePointer/977_LC_E_squares-of-a-sorted-array.cpp) | 字节， |
|        |                                                              |                                                              |        |
|        |                                                              |                                                              |        |





1. [344. 反转字符串](https://leetcode.cn/problems/reverse-string/description/) 双指针 [相关实现](01_DoublePointer/344_LC_E_reverse-string.cpp)
2. [977. 有序数组的平方](https://leetcode.cn/problems/squares-of-a-sorted-array/description/) [相关实现](01_DoublePointer/977_LC_E_squares-of-a-sorted-array.cpp) 双指针/直接计算+排序
3. [125. 验证回文](https://leetcode.cn/problems/valid-palindrome/description) [相关实现](01_DoublePointer/125_LC_E_valid-palindrome.cpp)

# 栈
 1. [1047. 删除字符串中的所有相邻重复项](https://leetcode.cn/problems/remove-all-adjacent-duplicates-in-string/description) [相关实现](03_Stack/1047_LC_E_remove-all-adjacent-duplicates-in-string.cpp)     栈

# Hash表
1. [1. 两数之和](https://leetcode.cn/problems/two-sum/description/) [相关实现](09_Hash/01_LC_E_two-sum.cpp)

# 排序
1. [347. 前 K 个高频元素](https://leetcode.cn/problems/top-k-frequent-elements/description) [相关实现](04_Heap/347_LC_M_top-k-frequent-elements.cpp)  快排/堆+ Hash表
2. [692. 前K个高频单词](https://leetcode.cn/problems/top-k-frequent-words/description) [相关实现](./04_Heap/692_LC_M_top-k-frequent-words.cpp) 快排/堆+Hash表

# 二分
1. [287. 寻找重复数](https://leetcode.cn/problems/find-the-duplicate-number/description) [相关实现](05_BinarySearch/278_LC_M_find-the-duplicate-number.cpp) 二分/位运算/快慢指针
2. [69. x 的平方根](https://leetcode.cn/problems/sqrtx/description/) [相关实现](05_BinarySearch/69_LC_E_sqrtx.cpp) 二分实现


# Floyd判圈算法(龟兔赛跑)
> <center>
>    <img src=https://pic4.zhimg.com/80/v2-0aedc55f3251cc12a4e6f2d2d9a4dc03_1440w.webp>
> </center>
>
> - 判断是否有环。定义两个指针p1与p2，起始时，都指向链表的起点A，p1每次移动1个长度，p2每次移动2个长度。如果p2在移到链表的尾端时，并未与p1相遇，表明链表中不存在环。如果p1与p2相遇在环上的某一点C，表明链表有环。
>
>- 环的长度。将指针p1固定在相遇位置C，移动p2，每次移动1个长度，并用变量cnt计数。当p2再次与p1相遇时，此时cnt的值就是环的长度。
>- 环的起点。即图中点B，将指针p1指向链表的起始位置A，指针p2仍在位置C，指针p1与p2每次均移动一个单位，p1与p2再次相遇的位置就是环的起点位置点B。
> 
> 参考资料：[知乎-弗洛伊德的兔子与乌龟](https://zhuanlan.zhihu.com/p/105269431)

1. [287. 寻找重复数](https://leetcode.cn/problems/find-the-duplicate-number/description) 二分/位运算/快慢指针 [相关实现](05_BinarySearch/278_LC_M_find-the-duplicate-number.cpp)
2. [141. 环形链表](https://leetcode.cn/problems/linked-list-cycle/description/) [相关实现](05_Floyd/141_LC_E_linked-list-cycle.cpp) 快慢指针
3. [142. 环形链表 II](https://leetcode.cn/problems/linked-list-cycle-ii/description/) [相关实现](05_Floyd/142_LC_M_linked-list-cycle-ii.cpp) 快慢指针


# 链表
1. [206. 反转链表](https://leetcode.cn/problems/reverse-linked-list/description/) [相关实现](00_LinkedList/206_LC_E_ReverseLinkedList.cpp) 单链表 + 迭代
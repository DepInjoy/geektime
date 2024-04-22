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

| 专题                                                     |                                                              |                                                              | 其他 |
| -------------------------------------------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ | ---- |
| 动态规划 `O(N^2)`<br/>贪心+二分查找 时间复杂度`O(NlogN)` | [300. 最长递增子序列](https://leetcode.cn/problems/longest-increasing-subsequence/) | [LC 300. 最长递增子序列](02_DP/300_LC_M_longest-increasing-subsequence.cpp) |      |
|                                                          |                                                              |                                                              |      |
|                                                          |                                                              |                                                              |      |





# 贪心

| 专题                                   | 题目                                                         | 相关实现                                                     | 其他 |
| -------------------------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ | ---- |
| 贪心                                   | [905. 区间选点](https://www.acwing.com/problem/content/description/907/) | [ACW 905. 区间选点](06_Greedy/950_ACW_M_interval-point-selection.cpp) |      |
| 贪心+二分查找<br/>时间复杂度`O(NlogN)` | [300. 最长递增子序列](https://leetcode.cn/problems/longest-increasing-subsequence/) | [LC 300. 最长递增子序列](02_DP/300_LC_M_longest-increasing-subsequence.cpp) |      |
|                                        |                                                              |                                                              |      |



# 回溯

| 专题 | 题目                                                    | 相关实现                                                   | 其他 |
| ---- | ------------------------------------------------------- | ---------------------------------------------------------- | ---- |
| 回溯 | [47.全排列II](03_BackTrack/47_LC_M_permutations-ii.cpp) | [LC 47.全排列II](03_BackTrack/47_LC_M_permutations-ii.cpp) |      |
|      |                                                         |                                                            |      |
|      |                                                         |                                                            |      |

# 图

| 专题 | 题目                                                    | 相关实现                                                   | 其他 |
| ---- | ------------------------------------------------------- | ---------------------------------------------------------- | ---- |
| 并查集| [1971. 寻找图中是否存在路径](https://leetcode.cn/problems/find-if-path-exists-in-graph/description/) | [LC 1971. 寻找图中是否存在路径](01_01_UnionFind/1973_LC_E_find-if-path-exists-in-graph.cpp) |   并查集实现含路径压缩和按秩归并，实现可做模板   |
| 深度/宽度优先搜索 | [面试题 04.01. 节点间通路](https://leetcode.cn/problems/route-between-nodes-lcci/) | [面试题 04.01. 节点间通路](000_Graph/04.01_LC_M_route-between-nodes-lcci.cpp) | 有向图，可以和[1971. 寻找图中是否存在路径](https://leetcode.cn/problems/find-if-path-exists-in-graph/description/)对比<br/>程序员面试金典(第 6 版) |
| 深度/宽度优先搜索 | [785. 判断二分图](https://leetcode.cn/problems/is-graph-bipartite/description/) | [LC 785. 判断二分图](000_Graph/785_LC_M_is-graph-bipartite.cpp) | 剑指Offer(专项突破)[106]<br/> |
| 深度/宽度优先搜索 | [695. 岛屿的最大面积](https://leetcode.cn/problems/max-area-of-island/description/) |[LC 695. 岛屿的最大面积](000_Graph/695_LC_M_max-area-of-island.cpp)| 剑指Offer(专项突破)[105]<br/>                |
| 多源最短路径(广度优先)<br/>动态规划 | [542. 01 矩阵](https://leetcode.cn/problems/01-matrix/) |[LC 542. 01 矩阵](000_Graph/542_LC_M_01-matrix.cpp)| 剑指Offer(专项突破)[107] |



# 树

| 专题       | 题目                                                         | 相关实现                                                   | 其他 |
| ---------- | ------------------------------------------------------------ | ---------------------------------------------------------- | ---- |
| 二叉树遍历 | [94. 二叉树的中序遍历](https://leetcode.cn/problems/binary-tree-inorder-traversal/description/)<br/>[144. 二叉树的前序遍历](https://leetcode.cn/problems/binary-tree-preorder-traversal/description/)<br/>[145. 二叉树的后序遍历](https://leetcode.cn/problems/binary-tree-postorder-traversal/description/)<br/>[102. 二叉树的层序遍历](https://leetcode.cn/problems/binary-tree-level-order-traversal/description/) | [LC 二叉树遍历](08_Tree/94_LC_E_binary-tree-traversal.cpp) |      |
|            |                                                              |                                                            |      |
|            |                                                              |                                                            |      |



# 前缀树

| 专题       | 题目                                                         | 相关实现                                                     | 其他 |
| ---------- | ------------------------------------------------------------ | ------------------------------------------------------------ | ---- |
| 前缀树+DFS | [212. 单词搜索 II](https://leetcode.cn/problems/word-search-ii/) | [LC 212. 单词搜索 II](01_00_Trie/212_LC_H_word-search-ii.cpp) |      |
|            |                                                              |                                                              |      |
|            |                                                              |                                                              |      |



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
| 双指针 | [977. 有序数组的平方](https://leetcode.cn/problems/squares-of-a-sorted-array/description/) | [LC 977. 有序数组的平方](01_DoublePointer/977_LC_E_squares-of-a-sorted-array.cpp) | 字节， |
|        |                                                              |                                                              |        |
|        |                                                              |                                                              |        |





1. [344. 反转字符串](https://leetcode.cn/problems/reverse-string/description/) 双指针 [相关实现](01_DoublePointer/344_LC_E_reverse-string.cpp)
2. [977. 有序数组的平方](https://leetcode.cn/problems/squares-of-a-sorted-array/description/) [相关实现](01_DoublePointer/977_LC_E_squares-of-a-sorted-array.cpp) 双指针/直接计算+排序
3. [125. 验证回文](https://leetcode.cn/problems/valid-palindrome/description) [相关实现](01_DoublePointer/125_LC_E_valid-palindrome.cpp)

# 栈

| 专题 | 题目                                                         | 实现                                                         | 其他 |
| ---- | ------------------------------------------------------------ | ------------------------------------------------------------ | ---- |
| 栈   | [1047. 删除字符串中的所有相邻重复项](https://leetcode.cn/problems/remove-all-adjacent-duplicates-in-string/description) | [LC 1047. 删除字符串中的所有相邻重复项](03_Stack/1047_LC_E_remove-all-adjacent-duplicates-in-string.cpp) |      |
| 栈   | [20. 有效的括号](https://leetcode.cn/problems/valid-parentheses/) | [LC 20. 有效的括号](03_Stack/20_LC_E_valid-parentheses.cpp)  |      |
|      |                                                              |                                                              |      |



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
| 专题                  | 题目                                                         | 实现                                                         | 其他 |
| --------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ | ---- |
| 单链表                | [206. 反转链表](https://leetcode.cn/problems/reverse-linked-list/description/) | [LC 206. 反转链表](00_LinkedList/206_LC_E_ReverseLinkedList.cpp) |      |
| 单链表<br/>栈，双指针 | [19. 删除链表的倒数第 N 个结点](https://leetcode.cn/problems/remove-nth-node-from-end-of-list/) | [LC 19. 删除链表的倒数第 N 个结点](00_LinkedList/19_LC_M_remove-nth-node-from-end-of-list.cpp) |      |
|                       |                                                              |                                                              |      |



# Hash表

| 专题          | 题目                                                         | 实现                                                         | 其他 |
| ------------- | ------------------------------------------------------------ | ------------------------------------------------------------ | ---- |
| Hash表+前缀和 | [523.连续的子数组和](https://leetcode.cn/problems/continuous-subarray-sum/description/) | [523.连续的子数组和](09_Hash/523_M_LC-continuous-subarray-sum.cpp)  [523.连续的子数组和](09_Hash/523_M_LC-continuous-subarray-sum.cpp) |      |
|               |                                                              |                                                              |      |
|               |                                                              |                                                              |      |


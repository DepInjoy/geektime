# 动态规划
1. [相关实现]() 01背包问题
2.  [相关实现]() 完全背包问题

---

| 专题                                                     |                                                              |                                                              | 其他                              |
| -------------------------------------------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ | --------------------------------- |
| <b><font color=FF3933>动态规划：01背包</font></b>        | [2. 01背包问题](https://www.acwing.com/problem/content/2/)   | [ACW 2. 01背包问题](02_DP/02_ACW_E_01-bag.cpp)               | <b>模版题</b>                     |
| <b><font color=FF3933>动态规划：完全背包</font></b>      | [3. 完全背包问题](https://www.acwing.com/problem/content/3/) | [ACW 3. 完全背包问题](02_DP/03_ACW_E_full-bag.cpp)           | <b>模版题</b>                     |
| 动态规划 `O(N^2)`<br/>贪心+二分查找 时间复杂度`O(NlogN)` | [300. 最长递增子序列](https://leetcode.cn/problems/longest-increasing-subsequence/) | [LC 300. 最长递增子序列](02_DP/300_LC_M_longest-increasing-subsequence.cpp) | <font color=FF3933>Hot 100</font> |
| 动态规划                                                 | [121. 买卖股票的最佳时机](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock/description/) | [LC 121. 买卖股票的最佳时机](02_DP/121_LC_E_best-time-to-buy-and-sell-stock.cpp) | <font color=FF3933>Hot 100</font> |
| 1. 动态规划<br/>2. 中心扩散法<br/>3. Manacher算法(TODO)  | [5. 最长回文子串]()                                          | [5. 最长回文子串](02_DP/05_LC_M_longest-palindromic-substring.cpp) | <font color=FF3933>Hot 100</font> |
| 动态规划                                                 | [53. 最大子数组和](https://leetcode.cn/problems/maximum-subarray/description/) | [LC 53. 最大子数组和](02_DP/53_LC_M_maximum-subarray.cpp)    | <font color=FF3933>Hot 100</font> |
| 动态规划                                                 | [213. 打家劫舍 II](https://leetcode-cn.com/problems/house-robber-ii/) | [LC 213. 打家劫舍 II](02_DP/213_LC_M_house-robber-ii.cpp)    |                                   |
| 动态规划+空间压缩                                        | [122. 买卖股票的最佳时机 II](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-ii/description) | [LC 122. 买卖股票的最佳时机 II](02_DP/122_LC_M_best-time-to-Buy-And-Sell-Stock-ii.cpp) |                                   |
| 01背包问题                                               | [416. 分割等和子集](https://leetcode.cn/problems/partition-equal-subset-sum/description/) | [LC 416. 分割等和子集](02_DP/416_LC_M_partition-equal-subset-sum.cpp) |                                   |
| 完全背包(组合数问题)                                     | [518. 零钱兑换 II](https://leetcode.cn/problems/coin-change-ii/description/) | [LC 518. 零钱兑换 II](02_DP/518_LC_M_coin-change-ii.cpp)     |                                   |
| 线性DP                                                   | [72. 编辑距离](https://leetcode.cn/problems/edit-distance/)  | [LC 72. 编辑距离](02_DP/72_LC_M_edit-distance.cpp)           |                                   |
| 动态规划                                                 | [377. 组合总和 Ⅳ](https://leetcode.cn/problems/combination-sum-iv/description/) | [LC 377. 组合总和 Ⅳ](02_DP/377_LC_M_combination-sum-iv.cpp)  |                                   |
|                                                          | [10. 正则表达式匹配](https://leetcode.cn/problems/regular-expression-matching/description/) | [LC 10. 正则表达式匹配](02_DP/10_LC_H_regular-expression-matching.cpp) | <font color=FF3933>Hot 100</font> |
|                                                          |                                                              |                                                              |                                   |





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
| 回溯 | [46. 全排列](https://leetcode.cn/problems/permutations/)|  [LC 46. 全排列](03_BackTrack/46_LC_M_permutations.cpp) | <font color=FF3933>Hot 100</font> |
| 回溯 | [39. 总组合数](https://leetcode.cn/problems/combination-sum/) | [LC 39. 总组合数](03_BackTrack/39_LC_M_combination-sum.cpp) |      |

# 图

| 专题 | 题目                                                    | 相关实现                                                   | 其他 |
| ---- | ------------------------------------------------------- | ---------------------------------------------------------- | ---- |
| 并查集| [1971. 寻找图中是否存在路径](https://leetcode.cn/problems/find-if-path-exists-in-graph/description/) | [LC 1971. 寻找图中是否存在路径](01_01_UnionFind/1973_LC_E_find-if-path-exists-in-graph.cpp) |   并查集实现含路径压缩和按秩归并，实现可做模板   |
| 深度/宽度优先搜索 | [面试题 04.01. 节点间通路](https://leetcode.cn/problems/route-between-nodes-lcci/) | [面试题 04.01. 节点间通路](000_Graph/04.01_LC_M_route-between-nodes-lcci.cpp) | 有向图，可以和[1971. 寻找图中是否存在路径](https://leetcode.cn/problems/find-if-path-exists-in-graph/description/)对比<br/>程序员面试金典(第 6 版) |
| 深度/宽度优先搜索 | [785. 判断二分图](https://leetcode.cn/problems/is-graph-bipartite/description/) | [LC 785. 判断二分图](000_Graph/785_LC_M_is-graph-bipartite.cpp) | 剑指Offer(专项突破)[106]<br/> |
| 深度/宽度优先搜索 | [695. 岛屿的最大面积](https://leetcode.cn/problems/max-area-of-island/description/) |[LC 695. 岛屿的最大面积](000_Graph/695_LC_M_max-area-of-island.cpp)| 剑指Offer(专项突破)[105]<br/>                |
| 多源最短路径(广度优先)<br/>动态规划 | [542. 01 矩阵](https://leetcode.cn/problems/01-matrix/) |[LC 542. 01 矩阵](000_Graph/542_LC_M_01-matrix.cpp)| 剑指Offer(专项突破)[107] |
| 深度/宽度优先搜索<br/>并查集 | [200.岛屿数量](https://leetcode.cn/problems/number-of-islands/description/) |[LC 200.岛屿数量](000_Graph/200_LC_M_number-of-islands.cpp)| <font color=FF3933>Hot 100</font> |



## 并查集

| 专题   | 题目                                                         | 相关实现                                                    | 其他                              |
| ------ | ------------------------------------------------------------ | ----------------------------------------------------------- | --------------------------------- |
| 并查集 | [200.岛屿数量](https://leetcode.cn/problems/number-of-islands/description/) | [LC 200.岛屿数量](000_Graph/200_LC_M_number-of-islands.cpp) | <font color=FF3933>Hot 100</font> |
|        |                                                              |                                                             |                                   |
|        |                                                              |                                                             |                                   |



# 树

| 专题                                                   | 题目                                                         | 相关实现                                                     | 其他                              |
| ------------------------------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ | --------------------------------- |
|                                                        |                                                              |                                                              |                                   |
| 二叉树遍历                                             | [94. 二叉树的中序遍历](https://leetcode.cn/problems/binary-tree-inorder-traversal/description/)<br/>[144. 二叉树的前序遍历](https://leetcode.cn/problems/binary-tree-preorder-traversal/description/)<br/>[145. 二叉树的后序遍历](https://leetcode.cn/problems/binary-tree-postorder-traversal/description/)<br/>[102. 二叉树的层序遍历](https://leetcode.cn/problems/binary-tree-level-order-traversal/description/) | [LC 二叉树遍历](08_Tree/94_LC_E_binary-tree-traversal.cpp)   |                                   |
|                                                        | [102. 二叉树的层序遍历](https://leetcode.cn/problems/binary-tree-level-order-traversal/description/) | [LC 102. 二叉树的层序遍历]()                                 | <font color=FF3933>Hot 100</font> |
| 1.递归实现<br/>2.非递归实现<br/>3.Morris中序遍历(TODO) | [94. 二叉树的中序遍历](https://leetcode.cn/problems/binary-tree-inorder-traversal/description/) | [LC 94. 二叉树的中序遍历](08_Tree/94_LC_E_binary-tree-inorder-traversal.cpp) | <font color=FF3933>Hot 100</font> |
|                                                        | [105. 从前序与中序遍历序列构造二叉树](https://leetcode.cn/problems/construct-binary-tree-from-preorder-and-inorder-traversal) | [105. 从前序与中序遍历序列构造二叉树](08_Tree/105_LC_M_construct-binary-tree-from-preorder-and-inorder-traversal.cpp) |                                   |
|                                                        | [106. 从中序与后序遍历序列构造二叉树](https://leetcode.cn/problems/construct-binary-tree-from-inorder-and-postorder-traversal/) | [106. 从中序与后序遍历序列构造二叉树](08_Tree/106_LC_M_construct-binary-tree-from-inorder-and-postorder-traversal.cpp) |                                   |
| 递归实现<br/>Hash表存储父节点                          | [236. 二叉树的最近公共祖先](https://leetcode.cn/problems/lowest-common-ancestor-of-a-binary-tree/description/) | [LC 236. 二叉树的最近公共祖先](08_Tree/236_LC_M_lowest-common-ancestor-of-a-binary-tree.cpp) | <font color=FF3933>Hot 100</font> |
| 深度优先遍历                                           | [297. 二叉树的序列化与反序列化](https://leetcode.cn/problems/serialize-and-deserialize-binary-tree/description/) | [LC 297. 二叉树的序列化与反序列化](08_Tree/297_LC_M_serialize-and-deserialize-binary-tree.cpp) |                                   |





# 前缀树

| 专题       | 题目                                                         | 相关实现                                                     | 其他 |
| ---------- | ------------------------------------------------------------ | ------------------------------------------------------------ | ---- |
| 前缀树+DFS | [212. 单词搜索 II](https://leetcode.cn/problems/word-search-ii/) | [LC 212. 单词搜索 II](01_00_Trie/212_LC_H_word-search-ii.cpp) |      |
| 前缀树| [208. 实现 Trie (前缀树)](https://leetcode.cn/problems/implement-trie-prefix-tree/) | [LC 208. 实现 Trie (前缀树)](01_00_Trie/208_LC_M_implement-trie-prefix-tree.cpp)|<b><font color=FF3933>Hot 100</font></b>|
|            |                                                              |                                                              |      |



# 堆

100  [相关实现]() 

| 专题                        | 题目                                                         | 实现                                                         | 其他                                     |
| --------------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ | ---------------------------------------- |
| 堆                          | [838. 堆排序-ACW](https://www.acwing.com/problem/content/840/) | [ACW 838. 堆排序-ACW](04_Heap/838_ACW_E_heap-sort.cpp)       | 手写堆(优化建堆时间复杂度为`O(N))`       |
| 快排<br/>堆                 | [215. 数组中的第K个最大元素](https://leetcode.cn/problems/kth-largest-element-in-an-array/description/) | [LC 215. 数组中的第K个最大元素](04_Heap/215_LC_M_kth-largest-element-in-an-array.cpp) | <b><font color=FF3933>Hot 100</font></b> |
| 堆<br/>快排+Hash表          | [347. 前 K 个高频元素](https://leetcode.cn/problems/top-k-frequent-elements/description) | [LC 347. 前 K 个高频元素](04_Heap/347_LC_M_top-k-frequent-elements.cpp) | <b><font color=FF3933>Hot 100</font></b> |
| 堆+Hash表<br/>Hash表 + 快排 | [692. 前K个高频单词](https://leetcode.cn/problems/top-k-frequent-words/description) | [LC 692. 前K个高频单词](04_Heap/692_LC_M_top-k-frequent-words.cpp) |                                          |
| 堆                          | [703. 数据流中的第 K 大元素](https://leetcode.cn/problems/kth-largest-element-in-a-stream/description/) | [LC 703. 数据流中的第 K 大元素](04_Heap/703_LC_E_kth-largest-element-in-a-stream.cpp) | 剑指Offer(专项突破)[59]                  |
| 堆                          | [23. 合并 K 个升序链表](https://leetcode.cn/problems/merge-k-sorted-lists/description/) | [LC 23. 合并 K 个升序链表](04_Heap/23_LC_H_merge-k-sorted-lists.cpp) | <b><font color=FF3933>Hot 100</font></b> |
|                             | [295. 数据流的中位数](https://leetcode.cn/problems/find-median-from-data-stream/) | [LC 295. 数据流的中位数]()                                   | <b><font color=FF3933>Hot 100</font></b> |

# 双指针

| 专题                           | 题目                                                         | 实现                                                         | 其他                                     |
| ------------------------------ | ------------------------------------------------------------ | ------------------------------------------------------------ | ---------------------------------------- |
|                                |                                                              |                                                              |                                          |
| 双指针                         | [977. 有序数组的平方](https://leetcode.cn/problems/squares-of-a-sorted-array/description/) | [LC 977. 有序数组的平方](01_DoublePointer/977_LC_E_squares-of-a-sorted-array.cpp) | 字节，                                   |
| 双指针                         | [15.三数之和](https://leetcode.cn/problems/3sum/)            | [LC 15.三数之和](00_01_DoublePointer\15_LC_M_3Sums.cpp)      | <b><font color=FF3933>Hot 100</font></b> |
| 双指针<br/>动态规划<br/>单调栈 | [42. 接雨水(H)](https://leetcode-cn.com/problems/trapping-rain-water/) | [LC 42. 接雨水](00_01_DoublePointer/42_LC_H_trapping-rain-water.cpp) | <b><font color=FF3933>Hot 100</font></b> |
| 双指针                         | [344. 反转字符串](https://leetcode.cn/problems/reverse-string/description/) | [LC 344. 反转字符串](01_DoublePointer/344_LC_E_reverse-string.cpp) |                                          |
| 双指针<br/>直接计算+排序       | [977. 有序数组的平方](https://leetcode.cn/problems/squares-of-a-sorted-array/description/) | [LC 977. 有序数组的平方](01_DoublePointer/977_LC_E_squares-of-a-sorted-array.cpp) |                                          |
|                                | [125. 验证回文](https://leetcode.cn/problems/valid-palindrome/description) | [125. 验证回文](01_DoublePointer/125_LC_E_valid-palindrome.cpp) |                                          |

# 栈

| 专题 | 题目                                                         | 实现                                                         | 其他 |
| ---- | ------------------------------------------------------------ | ------------------------------------------------------------ | ---- |
| 栈   | [1047. 删除字符串中的所有相邻重复项](https://leetcode.cn/problems/remove-all-adjacent-duplicates-in-string/description) | [LC 1047. 删除字符串中的所有相邻重复项](03_Stack/1047_LC_E_remove-all-adjacent-duplicates-in-string.cpp) |      |
| 栈   | [20. 有效的括号](https://leetcode.cn/problems/valid-parentheses/) | [LC 20. 有效的括号](03_Stack/20_LC_E_valid-parentheses.cpp)  | <b><font color=FF3933>Hot 100</font></b> |
|  单调栈 |   [155. 最小栈](https://leetcode.cn/problems/min-stack/) | [LC 155. 最小栈](03_Stack/155_LC_E_min-stack.cpp)   | <b><font color=FF3933>Hot 100</font> <br/></b>剑指Offer:30 |
| 单调栈 | [496. 下一个更大元素 I](https://leetcode.cn/problems/next-greater-element-i/description/) | [LC 496. 下一个更大元素 I](03_Stack/496_LC_E_Next-Greater-Element-i.cpp) | |
| 单调栈 | [ACW 830. 单调栈](https://www.acwing.com/problem/content/832/) | [ACW 830. 单调栈](03_Stack/830_ACW_E_monotonic-stack.cpp) | |





# 排序

| 专题                | 题目                                                         | 实现                                                         | 其他                                     |
| ------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ | ---------------------------------------- |
| 快排<br/>堆+ Hash表 | [347. 前 K 个高频元素](https://leetcode.cn/problems/top-k-frequent-elements/description) | [LC 347. 前 K 个高频元素](04_Heap/347_LC_M_top-k-frequent-elements.cpp) | <b><font color=FF3933>Hot 100</font></b> |
| 快排<br/>堆+Hash表  | [692. 前K个高频单词](https://leetcode.cn/problems/top-k-frequent-words/description) | [LC 692. 前K个高频单词](04_Heap/692_LC_M_top-k-frequent-words.cpp) |                                          |
|                     | [215. 数组中的第K个最大元素](https://leetcode.cn/problems/kth-largest-element-in-an-array/) | [LC 215. 数组中的第K个最大元素]()                            |                                          |



# 二分

| 专题                         | 题目                                                         | 实现                                                         | 其他 |
| ---------------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ | ---- |
| 二分<br/>位运算<br/>快慢指针 | [287. 寻找重复数](https://leetcode.cn/problems/find-the-duplicate-number/description) | [LC 287. 寻找重复数](05_BinarySearch/278_LC_M_find-the-duplicate-number.cpp) |      |
| 二分                         | [69. x 的平方根](https://leetcode.cn/problems/sqrtx/description/) | [LC 69. x 的平方根](05_BinarySearch/69_LC_E_sqrtx.cpp)       |      |
| 二分                         | [704. 二分查找](https://leetcode.cn/problems/binary-search/) | [LC 704. 二分查找](05_BinarySearch/704_LC_E_binary-search.cpp) |      |




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

| 专题                                  | 题目                                                         | 实现                                                         | 其他                              |
| ------------------------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ | --------------------------------- |
| 1. 二分<br/>2. 位运算<br/>3. 快慢指针 | [287. 寻找重复数](https://leetcode.cn/problems/find-the-duplicate-number/description) | [LC 287. 寻找重复数](05_BinarySearch/278_LC_M_find-the-duplicate-number.cpp) |                                   |
| 快慢指针(Floyd判圈算法即龟兔赛跑)     | [141. 环形链表](https://leetcode.cn/problems/linked-list-cycle/description/) | [LC 141. 环形链表](00_LinkedList/141_LC_E_linked-list-cycle.cpp) | <font color=FF3933>Hot 100</font> |
| 快慢指针                              | [142. 环形链表 II](https://leetcode.cn/problems/linked-list-cycle-ii/description/) | [LC 142. 环形链表 II](00_LinkedList/142_LC_M_linked-list-cycle-ii.cpp) | <font color=FF3933>Hot 100</font> |



# 链表

| 专题                  | 题目                                                         | 实现                                                         | 其他 |
| --------------------- | ------------------------------------------------------------ | ------------------------------------------------------------ | ---- |
| 单链表                | [206. 反转链表](https://leetcode.cn/problems/reverse-linked-list/description/) | [LC 206. 反转链表](00_LinkedList/206_LC_E_ReverseLinkedList.cpp) | <font color=FF3933>Hot 100</font> |
| 单链表<br/>栈，双指针 | [19. 删除链表的倒数第 N 个结点](https://leetcode.cn/problems/remove-nth-node-from-end-of-list/) | [LC 19. 删除链表的倒数第 N 个结点](00_LinkedList/19_LC_M_remove-nth-node-from-end-of-list.cpp) | <font color=FF3933>Hot 100</font> |
| 单链表|  [21. 合并两个有序链表](https://leetcode.cn/problems/merge-two-sorted-lists/description/)| [LC 21. 合并两个有序链表](00_LinkedList/21_LC_E_Merge-Two-Sorted-Lists.cpp) | <font color=FF3933>Hot 100</font> |
| 单链表 | [2. 两数相加](https://leetcode.cn/problems/add-two-numbers/description/) | [LC 2. 两数相加](00_LinkedList/02_LC_M_add-two-numbers.cpp) | <font color=FF3933>Hot 100</font> |
| 快慢指针(Floyd判圈算法即龟兔赛跑) | [141. 环形链表](https://leetcode.cn/problems/linked-list-cycle/description/) | [LC 141. 环形链表](00_LinkedList/141_LC_E_linked-list-cycle.cpp) |  |
| 快慢指针 | [142. 环形链表 II](https://leetcode.cn/problems/linked-list-cycle-ii/description/) | [LC 142. 环形链表 II](00_LinkedList/142_LC_M_linked-list-cycle-ii.cpp) | <font color=FF3933>Hot 100</font> |
| 双链表+Hash表 | [146. LRU缓存](https://leetcode.cn/problems/lru-cache/) | [LC 146.LRU缓存](00_LinkedList/146_LC_M_LRU.cpp) | <font color=FF3933>Hot 100</font> |
| Hash表+链表<br/>双指针 | [160. 相交链表](https://leetcode.cn/problems/intersection-of-two-linked-lists) | [LC 160. 相交链表](00_LinkedList/160_LC_E_intersection-of-two-linked-lists.cpp) | <b><font color=FF3933>Hot 100</font></b>|

# Hash表

| 专题          | 题目                                                         | 实现                                                         | 其他                              |
| ------------- | ------------------------------------------------------------ | ------------------------------------------------------------ | --------------------------------- |
| Hash表+前缀和 | [523.连续的子数组和](https://leetcode.cn/problems/continuous-subarray-sum/description/) | [523.连续的子数组和](09_Hash/523_M_LC-continuous-subarray-sum.cpp) |                                   |
| Hash表        | [1. 两数之和](https://leetcode.cn/problems/two-sum/description/) | [LC 1. 两数之和](09_Hash/01_LC_E_two-sum.cpp)                | <font color=FF3933>Hot 100</font> |
|               |                                                              |                                                              |                                   |



# 滑动窗口

| 专题     | 题目                                                         | 实现                                                         | 其他                              |
| -------- | ------------------------------------------------------------ | ------------------------------------------------------------ | --------------------------------- |
| 滑动窗口 | [3. 无重复字符的最长子串](https://leetcode.cn/problems/longest-substring-without-repeating-characters/description/) | [LC 3. 无重复字符的最长子串](06_SlidWindow/03_LC_M_longest-substring-without-repeating-characters.cpp) | <font color=FF3933>Hot 100</font> |
|滑动窗口| [438. 找到字符串中所有字母异位词](https://leetcode.cn/problems/find-all-anagrams-in-a-string/)|   [LC 438. 找到字符串中所有字母异位词](06_SlidWindow/438_LC_M_find-all-anagrams-in-a-string.cpp)| <b><font color=FF3933>Hot 100</font></b>|
|          |                                                              |                                                              |                                   |


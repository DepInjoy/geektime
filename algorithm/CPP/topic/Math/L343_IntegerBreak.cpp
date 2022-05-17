/**
 * @brief   343. 整数拆分
 *          给定一个正整数 n ，将其拆分为 k 个 正整数 的和（ k >= 2 ），
 *          并使这些整数的乘积最大化。返回 你可以获得的最大乘积 。
 *          链接：https://leetcode.cn/problems/integer-break/
 * 
 *          实现思想：
 *              1. 动态规划, 时间复杂度O(N^2), 空间复杂度O(N)
 *              2. 优化动态规划,时间复杂度O(N), 空间复杂度O(N)
 *                  dp[i]=max(2×(i−2),2×dp[i−2],3×(i−3),3×dp[i−3])
 *              3. 数学
 */

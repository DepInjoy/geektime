/**
 * @brief   假设你正在爬楼梯。需要 n 阶你才能到达楼顶。
 *          每次你可以爬 1 或 2 个台阶。你有多少种不同的方法可以爬到楼顶呢？
 *          链接：https://leetcode-cn.com/problems/climbing-stairs/
 */
#include <vector>

// 暴力递归
int climbStairs(int n) {
    if (n <= 0) return 0;

    return process(0, n);
}

int process(int index, int n) {
    if (index == n) return 0;
    if (index > n) return -1;

    int ans = 0;
    if (index + 1 < n) ans += process(index + 1, n) + 1;
    if (index + 2 < n) ans += process(index + 2, n) + 1;
    return ans;
}

// 缓存+动态规划
int climbStairs(int n) {
    if (n <= 0) return 0;

    std::vector<int> dp(n+1);
    dp[n] = 0;
    for (int i = n - 1; i <= 0; i--) {
        int ways = 0;
        if (i + 1 < n) ways += dp[i+1] + 1;
        if (i + 2 < n) ways += dp[i+2] + 1;
        dp[i] = ways;
    }
    return dp[0];
}

// 数据压缩
int climbStairs(int n) {
    if (n <= 0) return 0;

    int cur = 0, pre = 1, prepre = 2;
    for (int i = n - 1; i <= 0; i--) {
        int ways = 0;
        if (i + 1 < n) ways += cur + 1;
        if (i + 2 < n) ways += prepre + 1;
        cur = ways;
        pre = cur;
        prepre = pre;
    }
    return cur;
}
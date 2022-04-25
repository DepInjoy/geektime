/**
 * @brief   假设你正在爬楼梯。需要 n 阶你才能到达楼顶。
 *          每次你可以爬 1 或 2 个台阶。你有多少种不同的方法可以爬到楼顶呢？
 *          链接：https://leetcode-cn.com/problems/climbing-stairs/
 * 
 *          实现方式：
 *              1. 暴力递归，有两种尝试方式，其一是从底走到顶，其二是从顶走到底
 *              2. 缓存+动态规划:时间复杂度O(N),空间复杂度O(N)
 *              3. 动态规划+数据压缩:时间复杂度O(N),空间复杂度O(1)
 *              4. 矩阵乘法:时间复杂度O(logN)，空间复杂度O(1)
 */
#include <vector>

using namespace std;

// 暴力递归, 从底到顶
int climbStairs(int n) {
    if (n <= 0) return 0;

    return process(n, n);
}

int process(int index, int n) {
    if (index == 1) return 1;
    if (index == 2) return 2;
    if (index <= 0) return 0;

    return process(index - 1, n) + process(index - 2, n);
}

// 暴力递归, 从顶到底
int climbStairs(int n) {
    if (n <= 0) return 0;

    return process(0, n);
}

int process(int index, int n) {
    if (index >= n) return 0;
    if (index == n - 2) return 2;
    if (index == n - 1) return 1;

    return process(index + 1, n) + process(index + 2, n);
}

// 缓存+动态规划(从底到顶实现)
int climbStairs(int n) {
    if (n <= 0) return 0;

    std::vector<int> dp(n+2);
    dp[1] = 1, dp[2] = 2;
    for (int i = 3; i <= n; i++) {
        dp[i] = dp[i-1] + dp[i-2];
    }
    return dp[n];
}

// 数据压缩
int climbStairs(int n) {
    if (n <= 0) return 0;

    int cur = 1, pre = 1, prepre = 0;
    for (int i = 1; i <= n; i++) {
        cur = pre + prepre;
        prepre = pre;
        pre = cur;
    }
    return cur;
}

int climbStairs(int n) {

}

vector<vector<int> > matrixPower() {

}
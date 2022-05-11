/**
 * @brief   62.不同路径
 *          
 *          一个机器人位于一个 m x n 网格的左上角 （起始点在下图中标记为 “Start” ）。
 *          机器人每次只能向下或者向右移动一步。机器人试图达到网格的右下角（在下图中标记为 “Finish” ）。
 *          问总共有多少条不同的路径？
 *          链接：https://leetcode.cn/problems/unique-paths
 * 
 *          实现思想：
 *              1. 动态规划(不在这里讨论)
 *                  状态转移方程：f(i,j)=f(i−1,j)+f(i,j−1)
 * 
 *              2. 数学组合
 *                  需要移动m+n-2次，其中有 m-1次向下移动，n-1次向右移动。
 *                  路径的总数，就等于从m+n-2次移动中选择m-1次向下移动的方案数即组合数       
 */

// 数学组合
int uniquePaths(int m, int n) {
    long long ans = 1;
    if (m > n) swap(m, n);
    for (int x = n, y = 1; y < m; x++, y++) {
        ans = ans * x / y;
    }
    return ans;
}
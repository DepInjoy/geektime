/**
 * 2. 01背包问题
 * https://www.acwing.com/problem/content/2/
 * 
 * 题目表述：
 * 有N件物品和一个容量是V的背包, 每件物品只能使用一次。
 * 第i件物品的体积是 vi，价值是 wi
 * 求解将哪些物品装入背包, 可使这些物品的总体积不超过背包容量,且总价值最大
 * 输出最大价值。
 * 
 * 状态表示dp[i, j]
 *     集合：从前i个物品中选择体积小于等于j的所有物品集合
 *     属性计算:物品价值最大值max
 * 
 * 状态计算:
 * -------------------------------------------
 * |      sub1        |         sub2         |
 * -------------------------------------------
 * 
 * sub1：不含第i个物品, dp[i-1][j]
 * sub2: 含第i个物品, dp[i-1][j-vi] + wi
 * 
 * 由此，状态转移方程为:
 *     dp[i][j] = max(dp[i-1][j], dp[i-1][j-vi] + wi)
 * 
 * 从大到小遍历j将其压缩到一维
 *     dp[j] = max(dp[j], dp[j - vi] + wi)
*/

// -------------------- 1. 滚动数组实现，减低空间复杂度 --------------------
#include <vector>
#include <iostream>

int main() {
    int m, n;
    scanf("%d%d", &m, &n);
    std::vector<int> v(m), w(m);
    for (int i = 0; i < m; ++i) scanf("%d%d", &v[i], &w[i]);
    
    std::vector<int> dp(n + 1);
    for (int i = 0; i < m; ++i) {
        for (int j = n; j >= v[i]; --j) {
            dp[j] = std::max(dp[j], dp[j - v[i]] + w[i]);
        }
    }
    std::cout << dp[n];

    return 0;
}

// -------------------- 2. 二维空间, 原始数据实现 --------------------
#include <vector>
#include <iostream>

int main() {
    int m, n;
    scanf("%d%d", &m, &n);
    std::vector<int> v(m), w(m);
    for (int i = 0; i < m; ++i) scanf("%d%d", &v[i], &w[i]);
    
    std::vector<std::vector<int>> dp(m+1, std::vector<int>(n+1, 0));
    for (int i = 0; i < m; ++i) {
        for (int j = 1; j <= n; ++j) {
            dp[i+1][j] = dp[i][j];
            if (j >= v[i]) dp[i+1][j] = std::max(dp[i+1][j], dp[i][j-v[i]] + w[i]);
        }
    }
    std::cout << dp[m][n];
    return 0;
}
// ------------------------------------------------------------------
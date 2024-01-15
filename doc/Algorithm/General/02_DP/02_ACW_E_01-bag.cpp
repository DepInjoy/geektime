/**
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
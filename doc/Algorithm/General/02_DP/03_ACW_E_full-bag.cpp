/**
 *  3. 完全背包问题
 *  https://www.acwing.com/problem/content/3/
 *  
 *  状态表示dp[i, j]
 * - 集合：从前i个物品中选择提及小于等于j的所有物品集合
 * - 属性计算:物品价值最大值max
 * 
 * 状态计算:
 * -------------------------------------------------------
 * |  sub0  | sub1  |  sub2  |  ... |  subk  |  ....     |
 * -------------------------------------------------------
 * 
 * sub0：不含第i个物品, dp[i-1][j]
 * sub1: 含1个第i个物品, dp[i-1][j-vi] + wi
 * sub1: 含1个第i个物品, dp[i-1][j-2*vi] + wi*2
 *      ...
 * sub1: 含1个第i个物品, dp[i-1][j-k*vi] + wi*k
 * ....
 * 
 * 由此，状态转移方程为:
 * dp[i][j] = max(dp[i-1][j], dp[i-1][j-vi] + wi, dp[i-1][j-2*vi] +2wi, .....,  dp[i-1][j-k*vi] +k*wi) -- 公式1
 * 
 * 用j-vi替换上述方程中的j可以得到
 * dp[i][j-vi] = max(dp[i-1][j-vi], dp[i-1][j-2*vi] + wi, dp[i-1][j-3*vi] +2wi, .....,  dp[i-1][j-(k+1)*vi] +k*wi) -- 公式2
 * 
 * 因此, 公式1可以转化为：
 * dp[i][j] = max(dp[i-1][j], dp[i][j-vi]+wi)
 * 
 * 将j从0到m遍历，可以进行空间压缩
 * dp[j] = max(dp[j], dp[j-v[i]]+w[i])
*/

#include <vector>
#include <iostream>

int main() {
    int m, n;
    scanf("%d%d", &m, &n);
    std::vector<int> v(m), w(m);
    for (int i = 0; i < m; ++i) scanf("%d%d", &v[i], &w[i]);
    
    std::vector<int> dp(n+1);
    for (int i = 0; i < m; ++i) {
        for (int j = v[i]; j <= n; ++j) {
            dp[j] = std::max(dp[j], dp[j - v[i]] + w[i]);
        }
    }
    std::cout << dp[n];

    return 0;
}
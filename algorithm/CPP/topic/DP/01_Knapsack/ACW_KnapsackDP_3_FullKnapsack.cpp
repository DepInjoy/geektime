/**
 * @brief   完全背包
 *          有 N 种物品和一个容量是 V 的背包，每种物品都有无限件可用。
 *          第 i 种物品的体积是 vi，价值是 wi。求解将哪些物品装入背包
 *          可使这些物品的总体积不超过背包容量，且总价值最大。输出最大价值。
 *          链接：https://www.acwing.com/problem/content/3/
 */

#include <iostream>
#include <vector>
using namespace std;

const int N = 1010;
int v[N], w[N];
int dp[N];

int main() {
    int n, m;
    cin >> n >> m;
    for (int i = 0; i < n; i++) {
        cin >> v[i] >> w[i];
    }

    // 动态规划实现主体
    for (int i = 0; i < n; i++) {
        for (int j = v[i]; j <= m; j++) {
            dp[j] = max(dp[j], dp[j - v[i]] + w[i]);
        }
    }
    cout << dp[m];
    return 0;
}

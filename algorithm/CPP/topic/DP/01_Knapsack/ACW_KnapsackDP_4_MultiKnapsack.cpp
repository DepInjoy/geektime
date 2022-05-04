/**
 * @brief   多重背包问题
 *          有 N 种物品和一个容量是 V 的背包。
 *          第 i 种物品最多有 si 件，每件体积是 vi，价值是 wi。
 *          求解将哪些物品装入背包，可使物品体积总和不超过背包容量，且价值总和最大。
 *          输出最大价值。
 *          链接：https://www.acwing.com/problem/content/4/
 *          链接：https://www.acwing.com/problem/content/5/(多重背包的二进制优化)
 *          链接：https://www.acwing.com/problem/content/6/(多重背包的单调队列优化)
 */

#include <iostream>
#include <vector>
using namespace std;

const int N = 110;
int v[N], w[N], s[N];
int dp[N];

int main() {
    int n, m;
    cin >> n >> m;
    for (int i = 0; i < n; i++) {
        cin >> v[i] >> w[i] >> s[i];
    }
    
    // 动态规划实现主体
    for (int i = 0; i < n; i++) {
        for (int j = m; j >= 0; j--) {
            for (int k = 1; k <= s[i] && j - k * v[i] >= 0; k++) {
                dp[j] = max(dp[j], dp[j - k * v[i]] + k * w[i]);
            }
        }
    }
    cout << dp[m];
    return 0;
}
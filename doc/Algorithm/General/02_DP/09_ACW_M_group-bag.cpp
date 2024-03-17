/**
 * 9. 分组背包问题
 * https://www.acwing.com/problem/content/9/
 * 
 * 题目描述:
 * 有N组物品和一个容量是V的背包. 每组物品有若干个,同一组内的物品最多只能选一个
 * 每件物品的体积是vij，价值是wij，其中i是组号，j是组内编号
 * 求解将哪些物品装入背包, 可使物品总体积不超过背包容量且总价值最大。
*/

#include <iostream>
#include <vector>

int main() {
    
    int m, n;
    scanf("%d%d", &m, &n);

    std::vector<int> dp(n+1);
    while (m--) {
        int s;
        scanf("%d", &s);
        std::vector<int> v(s), w(s);
        for (int i = 0; i < s; ++i) {
            scanf("%d%d", &v[i], &w[i]);
        }
        
        for (int i = n; i >= 0; --i) {
            for (int j = 0; j < s; ++j) {
                if (i >= v[j]) dp[i] = std::max(dp[i], dp[i - v[j]] + w[j]);
            }
        }
    }
    std::cout << dp[n];

    return 0;
}

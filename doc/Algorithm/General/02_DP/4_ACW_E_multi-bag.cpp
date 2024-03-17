/**
 * 4. 多重背包问题 I
 * https://www.acwing.com/problem/content/4/
 * 
 * 题目描述:
 * 有N种物品和一个容量是V的背包,第i种物品最多有si件, 每件体积是vi, 价值是wi
 * 求解将哪些物品装入背包, 可使物品体积总和不超过背包容量, 且价值总和最大. 输出最大价值
 * 0 < N; V ≤ 100; 0 < vi, wi , si ≤ 100
 * 
 * 
 * 5. 多重背包问题 II
 * https://www.acwing.com/problem/content/5/
 * 0 < N ≤ 1000, 0 < V ≤ 2000, 0 < vi,wi,si ≤ 2000
 * 
 * 数字1-6可以用 1,2，3进行表示，
 * 1 = 1
 * 2 = 2
 * 3 = 3
 * 4 = 1 + 3
 * 5 = 2 + 3
 * 6 = 1 + 2 + 3

因此可以将上述实现拆分为MN * (log(S)向上取整)

假设K=log(S)，可以将其拆分为， 1， 2, ...., 2^k, S - 2^k个01背包问题
*/

// -------------------- 原始实现: M*N*S --------------------
#include <iostream>
#include <vector>

int main() {
    int m, n;
    scanf("%d%d", &m, &n);
    
    std::vector<int> dp(n+1);
    for (int i = 0; i < m; ++i) {
        int v, w, s;
        scanf("%d%d%d", &v, &w, &s);
        for (int j = n; j >= v; --j) {
            for (int k = 1; k <= s && j >= k*v; ++k) {
                dp[j] = std::max(dp[j], dp[j - k * v] + k * w);
            }
        }
    }
    std::cout << dp[n];

    return 0;
}

// -------------------- 二进制拆分优化(II) --------------------
// 算法复杂度MN * (log(S)向上取整)
#include <iostream>
#include <vector>

struct Good {
    int v, w;
};

int main() {
    int m, n;
    scanf("%d%d", &m, &n);

    std::vector<Good> goods;
    for (int i = 0; i < m; ++i) {
        int v, s, w;
        scanf("%d%d%d", &v, &w, &s);
        for (int i = 1; i <= s; i <<= 1) {
            s -= i;
            goods.push_back({i * v, i * w});
        }
        if (s) goods.push_back({s * v, s * w});
    }
    
    std::vector<int> dp(n+1);
    for (auto iter : goods) {
        for (int j = n; j >= iter.v; --j) {
            dp[j] = std::max(dp[j], dp[j-iter.v] + iter.w);
        }
    }
    std::cout << dp[n];
    
    return 0;
}

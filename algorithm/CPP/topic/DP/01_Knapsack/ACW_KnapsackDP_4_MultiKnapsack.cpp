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

// 多重背包的二进制优化
#ifdef MULTI_KNAPSACK_BINARY_OPT
#include <iostream>
#include <vector>
using namespace std;

const int N = 12010, M= 2010;
int v[N], w[N];
int dp[M];

int main() {
    int n, m;
    cin >> n >> m;
    int cnt = 0;
    for (int i = 0; i < n; i++) {
        int vi, wi, si;
        cin >> vi >> wi >> si;
        for (int k = 1; k <= si; k *= 2) {
            v[++cnt] = vi * k;
            w[cnt] = wi * k;
            si -= k;
        }
        if (si > 0) {
            v[++cnt] = vi * si;
            w[cnt] = wi * si;
        }
    }

    /*转化为01背包问题*/
    n = cnt;
    for (int i = 1; i <= n; i++) {
        for (int j = m; j >= v[i]; j--) {
            dp[j] = max(dp[j], dp[j - v[i]] + w[i]);
        }
    }
    cout << dp[m];
    return 0;
}
#endif

// 多重背包问题的单调队列优化
#if 0
#include <iostream>
#include<cstring>

using namespace std;

const int N = 20010;

int n, m;
int f[N], g[N], q[N];

int main() {
    cin >> n >> m;
    for (int i = 0; i < n; i++) {
        int v, w, s;
        cin >> v >> w >> s;
        memcpy(g, f, sizeof(f));
        
        for (int j = 0; j < v; j++) {
            int hh = 0, tt = -1;
            for (int k = j; k <= m; k += v) { // k表示m%v的第几个数
                f[k] = g[k];
                // 最多s+1个元素，超出个数限制则移除队首元素
                if (hh <= tt && k-s*v > q[hh]) hh++;
                // 队首肯定是最大的
                if (hh <= tt) f[k] = max(f[k], g[q[hh]]+(k-q[hh])/v*w);
                //将k压入队列前，先把所有比它小的出队
                while(hh <= tt && g[q[tt]]-(q[tt]-j)/v*w <= g[k]-(k-j)/v*w) tt--;
                q[++tt] = k;
            }
        }
    }

    cout << f[m] << endl;
    return 0;
}
#endif
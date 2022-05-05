/**
 * @brief   分组背包问题
 *          有 N 组物品和一个容量是 V 的背包。
 *          每组物品有若干个，同一组内的物品最多只能选一个。
 *          每件物品的体积是 vij，价值是 wij，其中 i 是组号，j 是组内编号。
 *          求解将哪些物品装入背包，可使物品总体积不超过背包容量，且总价值最大。输出最大价值。
 *          
 *          输入格式：第一行有两个整数 N，V，用空格隔开，分别表示物品组数和背包容量。
 *          接下来有 N 组数据, 每组数据第一行有一个整数 Si，表示第 i 个物品组的物品数量；
 *          每组数据接下来有 Si 行，每行有两个整数 vij,wij，用空格隔开，分别表示第 i 个物品组的第 j 个物品的体积和价值；
 *          
 *          输出格式：输出一个整数，表示最大价值。
 *      
 *          数据范围：0<N,V≤100, 0<Si≤100, 0<vij,wij≤100
 *          链接：https://www.acwing.com/problem/content/9/
 */

#include <iostream>
using namespace std;
const int N = 110;

int v[N][N], w[N][N], s[N];
int dp[N];

int main() {
    int m, n;
    cin >> n >> m;
    
    for (int i = 0; i < n; i++) {
        cin >> s[i];
        for (int j = 0; j < s[i]; j++) {
            cin >> v[i][j] >> w[i][j];
        }
    }
    
    for (int i = 0; i < n; i++) {
        for (int j = m; j >= 0; j--) {
            for (int k = 0; k <= s[i]; k++) {
                if (j - v[i][k] >= 0) {
                    dp[j] = max(dp[j], dp[j - v[i][k]] + w[i][k]);
                }
            }
        }
    }
    cout << dp[m] << endl;
    return 0;
}
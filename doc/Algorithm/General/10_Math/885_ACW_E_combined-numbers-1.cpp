/**
 * 885. 求组合数 I
 * https://www.acwing.com/problem/content/887/
 * 
 * 分析：
 * -------------------------
 * |                       |
 * |        O              |
 * |-----------------------
 * 
 * 1. 含o,那么从剩余的a-1中选取b-1个物品
 * 2. 不含o，那么从剩余的a-1中选取b个物品
 * 
 * C[a,b] = C[a-1, b-1] + C[a-1,b]
*/

#include <iostream>

const int N = 2010, mod = 1E9 + 7;
int d[N][N];

void init() {
    for (int i = 0; i <= 2000; ++i) {
        for (int j = 0; j <= i; ++j) {
            d[i][j] = (j == 0 ? 1 : d[i-1][j] + d[i-1][j-1]) % mod;
        }
    }
}

int main() {
    int n;
    scanf("%d", &n);

    init();
    
    while(n--) {
        int a, b;
        scanf("%d%d", &a, &b);
        
        std::cout << d[a][b] << std::endl;
    }
    return 0;
}
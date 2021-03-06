/**
 * @brief       斐波那契数 （通常用 F(n) 表示）形成的序列称为 斐波那契数列 。
 *              该数列由 0 和 1 开始，后面的每一项数字都是前面两项数字的和。也就是：
 *              F(0) = 0，F(1) = 1 F(n) = F(n - 1) + F(n - 2)，其中 n > 1
 *              链接：https://leetcode-cn.com/problems/fibonacci-number
 * 
 *              实现思想：
 *                  1. 递归实现,复杂度为O(2^N)
 *                  2. 动态规划
 *                  3. 动态规划+滚动数组
 *                  4. 矩阵快速幂
 *                  5. 通项公式
 */
#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;
// 1. 递归实现
int fib(int n) {
    if (n <= 1) return n;
    if (n == 2) return 1;
    return fib(n-1)+fib(n-2);
}

// 2. 动态规划,f[n]只依赖f[n-1]和f[n-2]可以进行空间压缩
int fib(int n) {    
    vector<int> dp = vector<int>(n+2);
    dp[0] = 0, dp[1] = 1;
    for (int i = 2; i <= n; i++) {
        dp[i] = dp[i-1] + dp[i-2];
    }
    return dp[n];
}

// 3. 动态规划+滚动数组
int fib(int n) {
    if (n <= 1) return n;

    int pre = 1, prepre = 0;
    int cur = 1;
    for (int i = 2; i <= n; i++) {
        cur = pre + prepre;
        prepre = pre;
        pre = cur;
    }
    return cur;
}

// 4. 矩阵快速幂
int fib(int n) {
    if (n <= 0) return 0;
    if (n == 1 || n == 2) return n;

    vector<vector<long>> factor = {
        {1, 1},
        {1, 0}
    };
    vector<vector<long>> ans = matrixPower(factor, n-2);
    return ans[0][0] + ans[1][0];
}

vector<vector<long>> multiMatrix(const vector<vector<long>>& m1,
        const vector<vector<long>>& m2) {
    vector<vector<long>> ans(m1.size(), vector<long>(m2[0].size(), 0));
    for (int i = 0; i < m1.size(); i++) {
        for (int j = 0; j < m2[0].size(); j++) {
            for (int k = 0; k < m2.size(); k++) {
                ans[i][j] += m1[i][k] * m2[k][j];
            }
        }
    }
    return ans;
}

vector<vector<long> > matrixPower(const vector<vector<long>>& matrix, const int n) {
    vector<vector<long>> ans(matrix.size(), vector<long>(matrix[0].size(), 0));
    // 将结果设为单位阵
    for (int i = 0; i < matrix.size(); i++) {
        ans[i][i] = 1;
    }

    vector<vector<long>> tmp = matrix;
    for (int i = n; i != 0; i >>= 1) {
        if (i & 0x01) {
            ans = multiMatrix(ans, tmp);
        }
        tmp = multiMatrix(tmp, tmp);
    }
    return ans;
}

// 5. 通项公式
int fib(int n) {
    double sqrt5 = sqrt(5);
    double fibN = pow((1 + sqrt5) / 2, n) - pow((1 - sqrt5) / 2, n);
    return round(fibN / sqrt5);
}
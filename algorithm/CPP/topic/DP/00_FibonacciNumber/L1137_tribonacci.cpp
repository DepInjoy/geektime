/**
 * @brief   1137. 第 N 个泰波那契数
 *          泰波那契序列 Tn 定义如下： 
 *          T0=0, T1=1, T2=1, 且在n>= 0 的条件下Tn+3=Tn+Tn+1+Tn+2
 *          给你整数n，请返回第n个泰波那契数Tn的值
 *          链接：https://leetcode-cn.com/problems/n-th-tribonacci-number
 * 
 *          实现思想：
 *              1. 记忆化搜索+动态规划
 *              2. 矩阵快速幂
 */

#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

int tribonacci(int n) {
    if (n <= 1) return n;
    if (n == 2) return 1;

    int t0 = 0, t1 = 1, t2 = 1;
    int cur = 2;
    for (int i = 3; i <= n; i++) {
        cur = t0 + t1 + t2;
        t0 = t1;
        t1 = t2;
        t2 = cur;
    }
    return cur;
}

int tribonacci(int n) {
    if (n <= 0) return 0;
    if (n == 1 || n == 2) return 1;

    vector<vector<long>> factor = {
        {1, 1, 1},
        {1, 0, 0},
        {0, 1, 0}
    };
    vector<vector<long>> ans = matrixPower(factor, n-2);
    return ans[0][0] + ans[0][1];
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
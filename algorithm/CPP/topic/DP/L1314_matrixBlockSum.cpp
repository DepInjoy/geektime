/**
 * @brief:  给你一个 m x n 的矩阵 mat 和一个整数 k ，请你返回一个矩阵 answer
 *          其中每个 answer[i][j] 是所有满足下述条件的元素 mat[r][c] 的和： 
 *          i - k <= r <= i + k,
 *          j - k <= c <= j + k 且(r, c) 在矩阵内。
 *          链接：https://leetcode.cn/problems/matrix-block-sum
 * 
 *          实现思想：
 *              二维前缀和
*/

#include <vector>
using namespace std;

int getRect(const vector<vector<int>>& presum, int m, int n, int x, int y) {
    x = max(min(x, m), 0);
    y = max(min(y, n), 0);
    return presum[x][y];
}

vector<vector<int>> matrixBlockSum(vector<vector<int>>& mat, int k) {
    int M = mat.size(), N = mat[0].size();
    vector<vector<int>> P(M+1, vector<int>(N+1));
    for (int i = 1; i <= M; i++) {
        for (int j = 1; j <= N; j++) {
            P[i][j] = P[i-1][j] + P[i][j-1] - P[i-1][j-1] + mat[i-1][j-1];
        }
    }

    vector<vector<int>> ans(M, vector<int>(N));
    for(int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            ans[i][j] = getRect(P, M, N, i+k+1, j+k+1) -
                    getRect(P, M, N, i-k, j+k+1) -
                    getRect(P, M, N, i+k+1, j-k) +
                    getRect(P, M, N, i-k, j-k);
        }
    }
    return ans;
}
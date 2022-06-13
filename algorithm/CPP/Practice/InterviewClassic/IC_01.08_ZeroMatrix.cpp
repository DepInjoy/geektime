/**
 * @brief   面试题 01.08. 零矩阵
 * 
 *          编写一种算法，若M × N矩阵中某个元素为0，则将其所在的行与列清零。
 *          链接：https://leetcode.cn/problems/zero-matrix-lcci/
 */

// 先利用进行数组标记，再进行清零
void setZeroes(vector<vector<int>>& matrix) {
    int M = matrix.size(), N = matrix[0].size();
    std::vector<bool> row(M), col(N);
    for (int i = 0; i < M; ++i) {
        for (int j = 0; j < N; ++j) {
            if (matrix[i][j] == 0) {
                row[i] = col[j] = true;
            }
        }
    }

    for (int i = 0; i < M; i++) {
        for (int j = 0; j < N; j++) {
            if (row[i] || col[j]) matrix[i][j] = 0;
        }
    }
}
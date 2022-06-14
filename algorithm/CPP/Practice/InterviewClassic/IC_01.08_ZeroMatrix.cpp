/**
 * @brief   面试题 01.08. 零矩阵
 * 
 *          编写一种算法，若M × N矩阵中某个元素为0，则将其所在的行与列清零。
 *          链接：https://leetcode.cn/problems/zero-matrix-lcci/
 */
// 设置标志位，时间复杂度O(MN)，空间复杂度为O(1)
// 利用第1行和第1列存储标记数组，同时设置flag_col标记第一列是否有0
// 从后向前，避免第0行被重写导致的错误
void setZeroes(vector<vector<int>>& matrix) {
    int M = matrix.size(), N = matrix[0].size();
    bool flag_col = false;
    for (int i = 0; i < M; ++i) {
        if (matrix[i][0] == 0) flag_col = true;
        for (int j = 1; j < N; ++j) {
            if (!matrix[i][j]) matrix[i][0] = matrix[0][j] = 0;
        }
    }

    for (int i = M - 1; i >= 0; --i) {
        for (int j = 1; j < N; ++j) {
            if (!matrix[i][0] || !matrix[0][j]) matrix[i][j] = 0;
        }
        if (flag_col) matrix[i][0] = 0;
    }
}

// 先利用进行数组标记，再进行清零,空间复杂度为O(N+M)，时间复杂度为O(MN)
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
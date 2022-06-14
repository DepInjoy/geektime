/**
 * @brief   面试题 01.07. 旋转矩阵
 *          给你一幅由N×N矩阵表示的图像，其中每个像素的大小为4字节
 *          请你设计一种算法，将图像旋转 90 度。
 *          不占用额外内存空间能否做到？
 *          链接：https://leetcode.cn/problems/rotate-matrix-lcci/
 */

void rotate(vector<vector<int>>& matrix) {
    if (matrix.size() == 0 || matrix.size() != matrix[0].size()) return;

    int N = matrix.size();
    for (int layer = 0; layer <= N/2; ++layer) {
        int start = layer, end = N-1-start;
        for (int i = start; i < end; ++i) {
            int offset = i - start;
            int top = matrix[start][i];
            // 左下角移动到上边
            matrix[start][i] = matrix[end - offset][start];
            // 右下角移动到左边
            matrix[end - offset][start] = matrix[end][end-offset];
            // 右上到下
            matrix[end][end-offset] = matrix[i][end];
            matrix[i][end] = top;
        }
    }
}
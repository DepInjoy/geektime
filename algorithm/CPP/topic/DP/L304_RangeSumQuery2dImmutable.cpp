/**
 * @brief:      给定一个二维矩阵 matrix，以下类型的多个请求：
 *              计算其子矩形范围内元素的总和，该子矩阵的 左上角 为 (row1, col1) ，右下角 为 (row2, col2)
 *              实现 NumMatrix 类：
 *                  NumMatrix(int[][] matrix) 给定整数矩阵 matrix 进行初始化
 *                  int sumRegion(int row1, int col1, int row2, int col2) 返回 左上角 (row1, col1) 
 *                      右下角 (row2, col2) 所描述的子矩阵的元素 总和 
 *              链接：https://leetcode.cn/problems/range-sum-query-2d-immutable
 * 
 *              实现思想：
 *                  1. 二维前缀和
*/

#include <vector>
using namespace std;

// 二维前缀和
class NumMatrix {
public:
    NumMatrix(vector<vector<int>>& matrix) {
        int m = matrix.size(), n = matrix[0].size();
        presum.resize(m+1, vector<int>(n+1));
        for (int i = 1; i <= m; i++) {
            for (int j = 1; j <= n; j++) {
                presum[i][j] = presum[i-1][j] + presum[i][j-1] - presum[i-1][j-1] + matrix[i-1][j-1];
            }
        }
    }
    
    int sumRegion(int row1, int col1, int row2, int col2) {
        return presum[row2+1][col2+1] - presum[row1][col2+1] - presum[row2+1][col1] + presum[row1][col1]; 
    }

private:
    vector<vector<int>> presum;
};

/**
 * @brief:  1292. 元素和小于等于阈值的正方形的最大边长
            给你一个大小为 m x n 的矩阵 mat 和一个整数阈值 threshold。
            请你返回元素总和小于或等于阈值的正方形区域的最大边长；如果没有这样的正方形区域，则返回 0
            链接：https://leetcode.cn/problems/maximum-side-length-of-a-square-with-sum-less-than-or-equal-to-threshold

            实现思想：
                1. 二维前缀和
                   第三重循环的优化思想：
                    1.1 如果边长为k的区域已经超过阈值，便不再需要继续进行更大的区域k+1计算
                    1.2 由于是寻找最大边长，如果在(i, j)找到了k,那么在(i', j')
                        只需要计算k+1是否满足，无须计算<k的
                    
*/

int getRect(const vector<vector<int>>& presum, int x1, int y1, int x2, int y2) {
    return presum[x2][y2] - presum[x1-1][y2] - presum[x2][y1-1] + presum[x1-1][y1-1];
}

int maxSideLength(vector<vector<int>>& mat, int threshold) {
    int m = mat.size(), n = mat[0].size();
    vector<vector<int>> presum(m+1, vector<int>(n+1));
    for (int i = 1; i <= m; i++) {
        for(int j = 1; j <= n; j++) {
            presum[i][j] = presum[i-1][j] + presum[i][j-1] - presum[i-1][j-1] + mat[i-1][j-1];
        }
    }

    int ans = 0;
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            for (int k = ans + 1; k <= min(m, n); k++) {
                if (i+k-1 <= m && j+k-1 <= n &&
                        getRect(presum, i, j, i+k-1, j+k-1) <= threshold) {
                    ans++;
                } else {
                    break;
                }
            }
        }
    }
    return ans;
}

/**
 * 542. 01 矩阵
 * https://leetcode.cn/problems/01-matrix/
 * https://leetcode.cn/problems/2bCMpM/description/
*/

vector<vector<int>> updateMatrix(vector<vector<int>>& mat) {
    const int m = mat.size(), n = mat[0].size();
    std::vector<std::vector<bool>> visited(m ,std::vector<bool>(n));
    std::vector<std::vector<int>> ans(m ,std::vector<int>(n, INT_MAX));
    std::queue<std::pair<int, int>> q;
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            if (mat[i][j] == 0) {
                ans[i][j] = 0;
                visited[i][j] = true;
                q.push({i, j});
            }
        }
    }

    const std::vector<int> dx = {0, 1, 0, -1};
    const std::vector<int> dy = {1, 0, -1, 0};
    while (!q.empty()) {
        auto point = q.front();
        q.pop();
        for (int i = 0; i < 4; ++i) {
            int x = point.first + dx[i], y = point.second + dy[i];
            if (x >= 0 && x < m && y >= 0 && y < n && !visited[x][y]) {
                ans[x][y] = std::min(ans[x][y], ans[point.first][point.second] + 1);
                q.push({x, y});
                visited[x][y] = true;
            }
        }
    }
    return ans;
}
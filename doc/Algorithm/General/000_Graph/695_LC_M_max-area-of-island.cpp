/**
 * 695. 岛屿的最大面积
 * https://leetcode.cn/problems/max-area-of-island/description/
 * https://leetcode.cn/problems/ZL6zAn/description/
*/

int getArea(const vector<vector<int>>& grid, std::vector<std::vector<bool>>& visited, int i, int j) {
    std::queue<std::vector<int>> q;
    q.push({i, j});
    std::vector<int> di {0, 1, 0, -1};
    std::vector<int> dj {1, 0, -1, 0};
    const int m = grid.size(), n = grid[0].size();
    int ans = 0;
    while (!q.empty()) {
        int ci = q.front()[0], cj = q.front()[1];
        q.pop();
        visited[ci][cj] = true;
        ++ans;
        for (int i = 0; i < 4; ++i) {
            int ni = ci + di[i], nj = cj + dj[i];
            if (ni >= 0 && ni < m && nj >= 0 && nj < n && grid[ni][nj] && !visited[ni][nj]) {
                q.push({ni, nj});
                visited[ni][nj] = true;
            }
        }   
    }
    return ans;
}

int maxAreaOfIsland(vector<vector<int>>& grid) {
    const int m = grid.size(), n = grid[0].size();
    std::vector<std::vector<bool>> visited(m, std::vector<bool>(n, false));
    int maxArea = 0;
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            if (grid[i][j] && !visited[i][j]) {
                int area = getArea(grid, visited, i, j);
                maxArea = std::max(maxArea, area);
            }
        }
    }
    return maxArea;
}
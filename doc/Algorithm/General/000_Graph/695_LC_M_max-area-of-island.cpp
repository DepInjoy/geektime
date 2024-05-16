/**
 * 695. 岛屿的最大面积
 * https://leetcode.cn/problems/max-area-of-island/description/
 * https://leetcode.cn/problems/ZL6zAn/description/
 * 
 * 实现方式：
 *  1. 深度优先遍历
 *  2. 宽度优先遍历
*/
// 深度优先遍历
int getArea(vector<vector<int>>& grid, int i, int j) {
    if (i < 0 || i >= grid.size() || j < 0 ||
            j >= grid[0].size() || grid[i][j] == 0)
        return 0;

    int area = 1;
    grid[i][j] = 0;
    int dx[4] = {0, 1, 0, -1};
    int dy[4] = {1, 0, -1, 0};
    for (int k = 0; k < 4; ++k) {
        int x = i + dx[k], y = j + dy[k];
        area += getArea(grid, x, y);
    }
    return area;
}

int maxAreaOfIsland(vector<vector<int>>& grid) {
    int ans = 0;
    const int m = grid.size(), n = grid[0].size();
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            if (grid[i][j]) {
                ans = std::max(ans, getArea(grid, i, j));
            }
        }
    }
    return ans;
}

// 宽度优先遍历
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

// 宽度优先遍历，不用visited数组
int getArea(vector<vector<int>>& grid, int i, int j) {
    int area = 0;
    std::queue<std::pair<int, int>> q;
    int dx[4] = {0, 1, 0, -1};
    int dy[4] = {1, 0, -1, 0};
    int m = grid.size(), n = grid[0].size();
    q.push({i, j});
    while (!q.empty()) {
        auto [x, y] = q.front();
        q.pop();
        if (x < 0 || x >= m || y < 0 || y >= n || grid[x][y] == 0) continue;
        ++area;
        grid[x][y] = 0;
        for (int k = 0; k < 4; ++k) {
            int nx = x + dx[k], ny = y + dy[k];
            q.push({nx, ny});
        }
    }
    return area;
}

int maxAreaOfIsland(vector<vector<int>>& grid) {
    int ans = 0;
    const int m = grid.size(), n = grid[0].size();
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            if (grid[i][j]) {
                ans = std::max(ans, getArea(grid, i, j));
            }
        }
    }
    return ans;
}
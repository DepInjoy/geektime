/**
 *  200. 岛屿数量
 *  https://leetcode.cn/problems/number-of-islands/description/
 * 
 *  实现方式：
 *      1. 深度优先搜索
 *      2. 宽度优先搜索
 *      3. 并查集查找连通图
 */

// 1. 深度优先搜索
void dfs(vector<vector<char>>& grid, int i, int j) {
    const int m = grid.size();
    const int n = grid[0].size();

    grid[i][j] = '0';
    int dx[4] = {0, 1, 0, -1};
    int dy[4] = {-1, 0, 1, 0};
    for (int k = 0; k < 4; ++k) {
        int x = i + dx[k], y = j + dy[k];
        if (x >= 0 && x < m && y >= 0 && y < n && grid[x][y] == '1') {
            dfs(grid, x, y);
        }
    }
}

int numIslands(vector<vector<char>>& grid) {
    int m = grid.size();
    if (m == 0) return 0;
    int n = grid[0].size();
    int num_islans = 0;
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            if (grid[i][j] == '1') {
                num_islans++;
                dfs(grid, i, j);
            }
        }
    }
    return num_islans;
}

// 宽度优先遍历
int numIslands(vector<vector<char>>& grid) {
    int m = grid.size();
    if (m == 0) return 0;
    int n = grid[0].size();
    int num_islans = 0;
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            if (grid[i][j] == '1') {
                num_islans++;
                grid[i][j] = '0';
                std::queue<std::pair<int, int>> q;
                q.push({i, j});
                int dx[4] = {0, 1, 0, -1};
                int dy[4] = {1, 0, -1, 0};
                while (!q.empty()) {
                    auto [r, c] = q.front();
                    q.pop();
                    for (int k = 0; k < 4; ++k) {
                        int x = r + dx[k], y = c + dy[k];
                        if (x >= 0 && x < m && y >= 0 && y < n && grid[x][y] == '1') {
                            q.push({x, y});
                            grid[x][y] = '0';
                        }
                    }
                }
            }
        }
    }
    return num_islans;
}

// 3. 并查集
class UnionFind {
public:
    UnionFind(std::vector<std::vector<char>>& grid) : count{0} {
        int m = grid.size(), n = grid[0].size();
        parent.reserve(m*n), rank.reserve(m*n);
        for (int i = 0; i < m; ++i) {
            for (int j = 0; j < n; ++j) {
                if (grid[i][j] == '1') {
                    parent.push_back(i*n+j);
                    ++count;
                } else {
                    parent.push_back(-1);
                }
                rank.push_back(0);
            }
        }
    }

    int Find(int x) {
        if (parent[x] != x) {
            parent[x] = Find(parent[x]);
        }
        return parent[x];
    }

    void Union(int x, int y) {
        int rootx = Find(x), rooty = Find(y);
        if (rootx != rooty) {
            if (rank[rootx] > rank[rooty]) {
                parent[rooty] = rootx;
            } else if (rank[rooty] > rank[rootx]) {
                parent[rootx] = rooty;
            } else {
                parent[rooty] = rootx;
                rank[rootx]++;
            }
            --count;
        }
    }

    int getCount() const {
        return count;
    }
private:
    std::vector<int>    parent;
    std::vector<int>    rank;
    int                 count;
};

int numIslands(vector<vector<char>>& grid) {
    int m = grid.size();
    if (m == 0) return 0;
    int n = grid[0].size();

    UnionFind uf(grid);
    int dx[4] = {0, 1, 0, -1};
    int dy[4] = {1, 0, -1, 0};
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            if (grid[i][j] == '1') {
                for (int k = 0; k < 4; ++k) {
                    int x = i + dx[k], y = j + dy[k];
                    if (x >= 0 && x < m && y >= 0 && y < n && grid[x][y] == '1') {
                        uf.Union(i * n + j, x * n + y);
                    }
                }
            }
        }
    }
    return uf.getCount();
}
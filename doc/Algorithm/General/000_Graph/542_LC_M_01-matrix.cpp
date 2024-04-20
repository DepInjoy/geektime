/**
 * 542. 01 矩阵
 * https://leetcode.cn/problems/01-matrix/
 * https://leetcode.cn/problems/2bCMpM/description/
 * 
 * 1. 多源最短路径
 * 2. 动态规划,图中的点到达[i, j]有四种方式
 *      -----------------------------------------------
 *      |       向右向下        |       向左向下        |
 *      --------------------[i, j]---------------------
 *      |       向右向上        |       向左向上        |
 *      ----------------------------------------------- 
*/

vector<vector<int>> updateMatrix(vector<vector<int>>& mat) {
    const int m = mat.size(), n = mat[0].size();
    std::vector<std::vector<int>> dist(m, std::vector<int>(n));
    std::vector<std::vector<bool>> visited(m, std::vector<bool>(n));
    std::queue<std::pair<int, int>> q;
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            if (mat[i][j] == 0) {
                dist[i][j] = 0;
                q.push({i, j});
                visited[i][j] = true;
            }
        }
    }

    const int dx[4] = {0, 1, 0, -1};
    const int dy[4] = {1, 0, -1, 0};
    while (!q.empty()) {
        auto [x, y] = q.front();
        q.pop();
        for (int i = 0; i < 4; ++i) {
            int a = x + dx[i], b = y + dy[i];
            if (a >= 0 && a < m && b >= 0 && b < n && !visited[a][b]) {
                dist[a][b] = dist[x][y] + 1;
                visited[a][b] = true;
                q.push({a, b});
            }
        }
    }
    return dist;
}

// 1. 动态规划
vector<vector<int>> updateMatrix(vector<vector<int>>& mat) {
    const int m = mat.size(), n = mat[0].size();
    std::vector<std::vector<int>> dist(m, std::vector<int>(n, INT_MAX/2));
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            if (mat[i][j] == 0) dist[i][j] = 0;
        }
    }

    // 1. 向左向上,达到[i, j]
    for (int i = m - 1; i >= 0; --i) {
        for (int j = n - 1; j >= 0; --j) {
            if (i + 1 < m) dist[i][j] = std::min(dist[i][j], dist[i+1][j] + 1);
            if (j + 1 < n) dist[i][j] = std::min(dist[i][j], dist[i][j+1] + 1);
        }
    }

    // 2. 向左+向下
    for (int i = m - 1; i >= 0; --i) {
        for (int j = 0; j < n; ++j) {
            if (i + 1 < m) dist[i][j] = std::min(dist[i][j], dist[i+1][j] + 1);
            if (j - 1 >= 0) dist[i][j] = std::min(dist[i][j], dist[i][j-1] + 1);
        }
    }

    // 3. 向右+向上
    for (int i =  0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i - 1 >= 0) dist[i][j] = std::min(dist[i][j], dist[i-1][j] + 1);
            if (j + 1 < n) dist[i][j] = std::min(dist[i][j], dist[i][j+1] + 1);
        }
    }

    // 4. 向右+向下
    for (int i = 0; i < m; ++i) {
        for (int j = n - 1; j >= 0; --j) {
            if (i - 1 >= 0) dist[i][j] = std::min(dist[i][j], dist[i-1][j] + 1);
            if (j - 1 >= 0) dist[i][j] = std::min(dist[i][j], dist[i][j-1] + 1);
        }
    }

    return dist;
}

// 动态规划,计算从1的点[i1,j1]移动到0点[i0, j0]
vector<vector<int>> updateMatrix(vector<vector<int>>& mat) {
    const int m = mat.size(), n = mat[0].size();
    std::vector<std::vector<int>> dist(m, std::vector<int>(n, INT_MAX-1));
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            if (mat[i][j] == 0) {
                dist[i][j] = 0;
            }
        }
    }
    
    // [i1, j1]向左移动，向上移动到达[i0, j0]
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            if (i - 1 >= 0) dist[i][j] = std::min(dist[i][j], dist[i-1][j] + 1);
            if (j - 1 >= 0) dist[i][j] = std::min(dist[i][j], dist[i][j-1] + 1);
        }
    }

    // [i1, j1]向左移动，向下移动到达[i0, j0]
    for (int i = 0; i < m; ++i) {
        for (int j = n-1; j < 0; --j) {
            if (i - 1 >= 0) dist[i][j] = std::min(dist[i][j], dist[i-1][j] + 1);
            if (j + 1 < n) dist[i][j] = std::min(dist[i][j], dist[i][j+1] + 1);
        }
    }

    // [i1, j1]向右移动，向上移动到达[i0, j0]
    for (int i = m - 1; i >= 0; --i) {
        for (int j = 0; j < n; ++j) {
            if (i + 1 < m) dist[i][j] = std::min(dist[i][j], dist[i+1][j] + 1);
            if (j - 1 >= 0) dist[i][j] = std::min(dist[i][j], dist[i][j-1] + 1);
        }
    }

    // [i1, j1]向右移动，向下移动到达[i0, j0]
    for (int i = m - 1; i >= 0; --i) {
        for (int j = n - 1; j >= 0; --j) {
            if (i + 1 < m) dist[i][j] = std::min(dist[i][j], dist[i+1][j] + 1);
            if (j + 1 < n) dist[i][j] = std::min(dist[i][j], dist[i][j+1] + 1);
        }
    }
    return dist;
}

/**
 *  动态规划的常数优化
 * 上述实现有一些重复计算的地方, 只需要保留
 *   1. 只有水平向左移动和竖直向上移动
 *   2. 只有水平向右移动和竖直向下移动
 *      |\     / |
 *      | \   /  |
 *      |   x    |
 * 
 * 引用Leetcode评论区的题解
 *     证明右上.在这给出一个性质:假如距离(i,j)最近的点在(i-a,j+b) a>0,b>0，则距离(i,j+b)最近的点在(i-a,j+b)
 *     用反证法证明： 如果距离(i,j+b)最近的点(x,y)不在(i-a,j+b), 则(i,j+b)和(x,y)距离d<a
 *     这时点(i,j)和(x,y)的距离d'<=b+d<a+b,与假设矛盾
 *     (动态规划的无后效性)
 */

vector<vector<int>> updateMatrix(vector<vector<int>>& mat) {
    const int m = mat.size(), n = mat[0].size();
    std::vector<std::vector<int>> dist(m, std::vector<int>(n, INT_MAX-1));
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            if (mat[i][j] == 0) {
                dist[i][j] = 0;
            } else {
                // 从左上移动
                if (i - 1 >= 0) dist[i][j] = std::min(dist[i][j], dist[i-1][j] + 1);
                if (j - 1 >= 0) dist[i][j] = std::min(dist[i][j], dist[i][j-1] + 1);
            }
        }
    }

    // 从右下移动
    for (int i = m - 1; i >= 0; --i) {
        for (int j = n - 1; j >= 0; --j) {
            if (i + 1 < m) dist[i][j] = std::min(dist[i][j], dist[i+1][j] + 1);
            if (j + 1 < n) dist[i][j] = std::min(dist[i][j], dist[i][j+1] + 1);
        }
    }
    return dist;
}

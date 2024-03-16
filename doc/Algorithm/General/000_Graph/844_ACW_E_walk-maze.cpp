/**
 * 844. 走迷宫
 * https://www.acwing.com/problem/content/description/846/
*/

#include <iostream>
#include <cstring>
#include <queue>

typedef std::pair<int, int> PII;
const int N = 110;
int d[N][N], t[N][N];

int bfs(int m, int n) {
    memset(t, -1, sizeof(t));
    std::queue<PII> q;

    q.push({0, 0});
    t[0][0] = 0;
    
    int dx[4] = {0, 1, 0, -1}, dy[4] = {1, 0, -1, 0};
    while (!q.empty()) {
        auto item = q.front();
        q.pop();

        for (int i = 0; i < 4; ++i) {
            int x = item.first + dx[i], y = item.second + dy[i];
            if (x >= 0 && x < m && y >= 0 && y < n && d[x][y] == 0 && t[x][y] == -1) {
                t[x][y] = t[item.first][item.second] + 1;
                q.push({x, y});
            }
        }
    }
    return t[m-1][n-1];
}

int main() {
    int m, n;
    scanf("%d%d", &m, &n);
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            scanf("%d", &d[i][j]);
        }
    }
    
    std::cout << bfs(m, n) << std::endl;
    return 0;
}
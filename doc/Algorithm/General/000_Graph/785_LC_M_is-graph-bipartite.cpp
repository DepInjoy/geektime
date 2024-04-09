/**
 * 785. 判断二分图
 * https://leetcode.cn/problems/is-graph-bipartite/description/
 * https://leetcode.cn/problems/vEAB3K/
 * 
 * 实现方式，尝试按照二分图规则给节点上色，上色中，如果不符合二分图规则，则直接返回false
 * 1. 宽度优先遍历
 * 2. 深度优先遍历
*/

// 1. 宽度优先遍历
bool setColor(const vector<vector<int>>& graph, std::vector<int> colors, int i, int color) {
    std::queue<int> q;
    q.push(i), colors[i] = 0;
    while (!q.empty()) {
        int u = q.front();
        q.pop();
        for (auto v : graph[u]) {
            if (colors[v] >= 0) {
                if (colors[u] == colors[v]) {
                    return false;
                }
            } else {
                colors[v] = 1 - colors[u];
                q.push(v);
            }
        }
    }
    return true;
}

bool isBipartite(vector<vector<int>>& graph) {
    const int n = graph.size();
    std::vector<int> colors(n, -1);
    for (int i = 0; i < n; ++i) {
        if (!setColor(graph, colors, i, 0)) {
            return false;
        }
    }
    return true;
}

// 2.深度优先遍历
bool setColor(const vector<vector<int>>& graph, std::vector<int>& colors, int i, int color) {
    if (colors[i] >= 0) return colors[i] == color;

    colors[i] = color;
    for (int j : graph[i]) {
        if (!setColor(graph, colors, j, 1 - color)) {
            return false;
        }
    }
    return true;
}

bool isBipartite(vector<vector<int>>& graph) {
    const int n = graph.size();
    std::vector<int> colors(n, -1);
    for (int i = 0; i < n; ++i) {
        if (colors[i] >= 0) continue;
        if (!setColor(graph, colors, i, 0)) {
            return false;
        }
    }
    return true;
}
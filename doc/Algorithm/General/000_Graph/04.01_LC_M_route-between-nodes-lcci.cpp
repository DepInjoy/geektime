/**
 * 面试题 04.01. 节点间通路
 * 
 * 节点间通路。给定有向图，设计一个算法，找出两个节点之间是否存在一条路径
 * https://leetcode.cn/problems/route-between-nodes-lcci/
*/

// 深度优先搜索
bool dfs(std::vector<std::unordered_set<int>>& edges, std::vector<bool>& visited, int node, int target) {
    if (edges[node].count(target)) return true;

    visited[node] = true;
    for (int next : edges[node]) {
        if (!visited[next]) {
            bool found = dfs(edges, visited, next, target);
            if (found) return true;
            visited[next] = true;
        }
    }
    return false;
}
bool findWhetherExistsPath(int n, vector<vector<int>>& graph, int start, int target) {
    if (start == target) return true;
    if (start >= n || target >= n) return false;

    std::vector<std::unordered_set<int>> edges(n);
    std::vector<bool> visited(n);
    for (auto edge : graph) {
        edges[edge[0]].insert(edge[1]);
    }

    return dfs(edges, visited, start, target);
}

// 宽度优先搜索
bool findWhetherExistsPath(int n, vector<vector<int>>& graph, int start, int target) {
    if (start == target) return true;
    if (start >= n || target >= n) return false;

    std::vector<std::unordered_set<int>> node_paths(n);
    for (auto edge : graph) {
        node_paths[edge[0]].insert(edge[1]);
    }

    std::queue<int> q;
    std::vector<bool> visited(n);
    q.push(start);
    visited[start] = true;
    while (!q.empty()) {
        int cur = q.front();
        q.pop();
        if (cur == target) return true;

        for (auto node : node_paths[cur]) {
            if (node == target) return true;
            if (!visited[node]) {
                q.push(node);
                visited[node] = true;
            }
        }
    }
    return false;
}
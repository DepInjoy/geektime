/**
 * @brief   面试题 04.01. 节点间通路
 *          节点间通路。给定有向图，设计一个算法，找出两个节点之间是否存在一条路径。
 *          链接：https://leetcode.cn/problems/route-between-nodes-lcci/
 * 
 */

bool findWhetherExistsPath(int n, vector<vector<int>>& graph, int start, int target) {
    if (start == target) return true;
    if (start >= n || target >= n) return false;

    std::vector<std::unordered_set<int>> tmp(n);
    for (int i = 0; i < graph.size(); ++i) {
        tmp[graph[i][0]].insert(graph[i][1]);
    }

    std::unordered_set<int> visited;
    std::deque<int> q;
    q.push_back(start);
    visited.insert(start);
    while(!q.empty()) {
        int cur = q.front();
        if (cur == target) return true;
        for (int item : tmp[cur]) {
            if (item == target) return true;
            if (!visited.count(item)) {
                q.push_back(item);
                visited.insert(item);
            }
        }
        q.pop_front();
    }
    return false;
}
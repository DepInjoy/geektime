/**
 * 207. 课程表
 * https://leetcode.cn/problems/course-schedule/description/
 * 
 * 实现方法：
 *  1. 拓扑排序
*/


// 拓扑排序 + 广度优先遍历
bool canFinish(int numCourses, vector<vector<int>>& prerequisites) {
    std::vector<std::vector<int>> edges(numCourses);
    std::vector<int> indeg(numCourses);
    for (auto c : prerequisites) {
        edges[c[1]].emplace_back(c[0]);
        ++indeg[c[0]];
    }

    std::queue<int> q;
    for (int i = 0; i < numCourses; ++i) {
        if (indeg[i] == 0) q.push(i);
    }

    int visited = 0;
    while (!q.empty()) {
        ++visited;
        int u = q.front();
        q.pop();
        for (int v : edges[u]) {
            --indeg[v];
            if (indeg[v] == 0) q.push(v);
        }
    }
    return visited == numCourses;
}
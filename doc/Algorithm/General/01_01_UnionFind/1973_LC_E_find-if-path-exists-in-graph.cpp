/**
 * 1971. 寻找图中是否存在路径
 * https://leetcode.cn/problems/find-if-path-exists-in-graph/description/
*/

class UnionFind {
public:
    UnionFind(int n) : parent(n), rank(n) {
        for (int i = 0; i < n; ++i) {
            parent[i] = i;
        }
    }

    int find(int x) {
        if (parent[x] != x) {
            parent[x] = find(parent[x]);
        }
        return parent[x];
    }

    void union_uf(int x, int y) {
        int rootx = find(x), rooty = find(y);
        if (rootx != rooty) {
            if (rank[x] < rank[y]) {
                parent[rootx] = rooty;
            } else if (rank[x] > rank[y]) {
                parent[rooty] = rootx;
            } else {
                parent[rooty] = rootx;
                rank[rootx]++;
            }
        }
    }

    bool connected(int x, int y) {
        return find(x) == find(y);
    }
private:
    std::vector<int>    parent;
    std::vector<int>    rank;
};

bool validPath(int n, vector<vector<int>>& edges, int source, int destination) {
    if (source == destination) return true;

    UnionFind uf(n);
    for (auto edge : edges) {
        uf.union_uf(edge[0], edge[1]);
    }

    return uf.connected(source, destination);
}
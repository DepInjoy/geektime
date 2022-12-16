#include <vector>
#include <iostream>

class UnionSet {
public:
    explicit UnionSet(const int N);
    int Find(const int x);
    void Union(const int node1, const int node2);

private:
    // 存储每个点的根节点
    std::vector<int>            nodes;
    // 只有根节点的有意义，表示根节点所在集合中的点的数量
    std::vector<int>            sizes;
};

UnionSet::UnionSet(const int N) {
    for (int i = 0; i < N; ++i) {
        nodes.push_back(i);
        sizes.push_back(1);
    }
}

int UnionSet::Find(const int x) {
    if (nodes[x] != x) nodes[x] = Find(x);  // 路径压缩
    return nodes[x];
}

// 按秩归并，根据数据规模
void UnionSet::Union(const int node1, const int node2) {
    int parent1 = Find(node1);
    int parent2 = Find(node2);
    // 小树挂到大树上
    if (sizes[parent1] < sizes[parent2]) {
        nodes[parent1] = parent2;
        sizes[parent2] += sizes[parent1];
    } else {
        nodes[parent2] = parent1;
        sizes[parent1] += sizes[parent2];
    }
}

int main(int argc, char* argv[]) {
    return 0;
}


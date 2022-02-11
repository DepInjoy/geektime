package algorithm.training.struct.graph;

import java.util.*;

/**
 * @brief:              Kruskal算法
 *
 *              实现思想, 整体是贪心，不会先入局部最优解
 *                  1. 寻找权重最小的边
 *                  2. 以该点为中心寻找不会形成环的下一条最小边
 *                  3. 重复第2步, 直到所有的边都被访问过
 *
 *              时间复杂度:O(|E|log|E|)
 * */
public class D16_04_Kruskal {
    public static class UnionSet {
        // key: 节点 value: 父节点
        private HashMap<Node, Node> parentMap;
        // key: 节点 value:并查集节点规模
        private HashMap<Node, Integer> sizeMap;

        public UnionSet() {
            parentMap = new HashMap<>();
            sizeMap = new HashMap<>();
        }

        // 并查集初始化
        public void makeUnionSet(Collection<Node> nodes) {
            parentMap.clear();
            sizeMap.clear();
            for(Node node : nodes) {
                parentMap.put(node, node); // 节点指向自己
                sizeMap.put(node, 1); // 当前节点规模初始化为1
            }
        }

        public void union(Node a, Node b) {
            if (a == null || b == null) return;

            Node aparent = find(a);
            Node bparent = find(b);
            if (aparent != bparent) {
                Integer asize = sizeMap.get(aparent);
                Integer bsize = sizeMap.get(bparent);
                if (asize > bsize) {
                    parentMap.put(bparent, aparent);
                    sizeMap.put(aparent, asize + bsize);
                    sizeMap.remove(bparent);
                } else {
                    parentMap.put(aparent, bparent);
                    sizeMap.put(bparent, asize + bsize);
                    sizeMap.remove(aparent);
                }
            }
        }

        public Node find(Node node) {
            Stack<Node> stack = new Stack<>();
            Node parent = node;
            // 父节点是指向自身的节点
            while (parent != parentMap.get(node)) {
                stack.add(parent);
                parent = parentMap.get(node);
            }
            // 路径压缩
            while (!stack.isEmpty()) {
                parentMap.put(stack.pop(), parent);
            }
            return parent;
        }

        public boolean isSameSet(Node a, Node b) {
            return find(a) == find(b);
        }

    }

    public static class EdgeComparator implements Comparator<Edge> {
        @Override
        public int compare(Edge e1, Edge e2) {
            return e1.weight - e2.weight;
        }
    }

    public static Set<Edge> kruskalMST(Graph graph) {
        if (graph == null) return null;

        Set<Edge> result = new HashSet<Edge>();
        UnionSet unionSet = new UnionSet();
        unionSet.makeUnionSet(graph.nodes.values());
        PriorityQueue<Edge> priorityQueue = new PriorityQueue<>(new EdgeComparator());
        // 将各条边添加到小根堆，寻找最小边
        for (Edge edge : graph.edges) {
            priorityQueue.add(edge);
        }

        while (!priorityQueue.isEmpty()) {
            // 取出一条最小边
            Edge minEdge = priorityQueue.poll();
            // 利用并查集判断是否会形成回路，如果不会则加入结果，否则跳过
            if (!unionSet.isSameSet(minEdge.from, minEdge.to)) {
                result.add(minEdge);
                unionSet.union(minEdge.from, minEdge.to);
            }
        }
        return result;
    }
}

package algorithm.training.struct.graph;

import java.util.Comparator;
import java.util.HashSet;
import java.util.PriorityQueue;
import java.util.Set;

/**
 * @brief:      Prim最小生成树算法
 *
 *              实现思想：
 *                  1. 随机选一个点,选取周围weight最小的边
 *                  2. 确定这条边另外一个顶点没有访问过，将该顶点周围的边加入最小堆
 *                  3. 取出最小边，继续执行2,直到所有的顶点都被访问过
 *
 *              时间复杂度: O(V^2)
 * */
public class D16_05_Prim {
    public static class EdgeComparator implements Comparator<Edge> {
        @Override
        public int compare(Edge e1, Edge e2) {
            return e1.weight - e2.weight;
        }
    }

    // 生成最小生成树
    public static Set<Edge> primMST(Graph graph) {
        if (graph == null) return null;

        PriorityQueue<Edge> priorityQueue = new PriorityQueue<>(new EdgeComparator());
        HashSet<Node> visitedNodes = new HashSet<Node>();
        HashSet<Edge> result = new HashSet<>();
        // 随机选一个节点开始
        for (Node node : graph.nodes.values()) {
            if (!visitedNodes.contains(node)) {
                // 将节点周围的边添加到小根堆来寻找weight最小的边
                for (Edge edge : node.edges) {
                    priorityQueue.add(edge);
                }
                while (!priorityQueue.isEmpty()) {
                    // 取出weight最小的边
                    Edge minEdge = priorityQueue.poll();
                    if (!visitedNodes.contains(minEdge.to)) { // 确定不会形成环
                        result.add(minEdge);
                        visitedNodes.add(minEdge.to);
                        for (Edge edge : minEdge.to.edges) {
                            priorityQueue.add(edge);
                        }
                    }
                }
            }
        }
        return result;
    }

    // graph是连通图, graph[i][j]表示点i到点j的距离，如果是系统最大值代表无路
    // 返回值是最小连通图的路径之和
    public static int prim(int[][] graph) {
        if (graph == null || graph.length == 0) return 0;

        // 连接图表示图，长度和宽度相等
        int size = graph.length;
        int minSumDistance = 0;
        int[] distances = new int[size]; // 到index节点的最小距离
        boolean[] visited = new boolean[size]; // 节点是否访问过
        visited[0] = true;
        for (int i = 0; i < size;++i) {
            distances[i] = graph[0][i];
        }

        for (int i = 1; i < size; ++i) { // 从1号节点开始寻找它到其他节点的最小距离
            int minDis = Integer.MAX_VALUE;
            int minIndex = -1;
            // 寻找当前到i节点的最短路径节点
            for (int j = 0; j < size;++j) {
                if (!visited[j] && distances[j] < minDis) {
                    minDis = distances[j];
                    minIndex = j;
                }
            }
            // 所有节点都已经被访问过了
            if (minIndex == -1) return minSumDistance;

            visited[i] = true;
            minSumDistance += minDis;
            // 更新到节点的最小距离
            for (int j = 0; j < size; ++j) {
                if (!visited[j] && distances[j] < graph[i][j]) {
                    distances[j]= graph[i][j];
                }
            }
        }

        return minSumDistance;
    }
}

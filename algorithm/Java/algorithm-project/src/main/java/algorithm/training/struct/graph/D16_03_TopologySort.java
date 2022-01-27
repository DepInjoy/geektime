package algorithm.training.struct.graph;

import java.util.*;

/**
 * @brief:      拓扑排序
 *              实现思路:
 *                  1. 结果集为入度为0的节点, 其中节点每被访问一次入度减1
 * */
public class D16_03_TopologySort {
    /**
     *      @brief:     结果集为节点入度为0的节点
     *                  节点每被访问一次节点入度减1,入度为0, 则加入结果集
     * */
    public static List<Node> sortedTopology(Graph graph) {
        if (graph == null) return null;

        // 节点和入度映射表
        HashMap<Node, Integer> nodeInMap = new HashMap<Node, Integer>();
        // 入度为0的节点组成的队列
        Queue<Node> zeroInQueue = new LinkedList<Node>();
        for (Node node : graph.nodes.values()) {
            nodeInMap.put(node, node.in);
            if (node.in == 0) {
                zeroInQueue.add(node);
            }
        }

        List<Node> result = new ArrayList<Node>();
        while (!zeroInQueue.isEmpty()) {
            Node cur = zeroInQueue.poll();
            result.add(cur);
            for (Node next : cur.nexts) {
                // 节点每被访问一次,入度减1
                nodeInMap.put(next, nodeInMap.get(next)-1);
                if (nodeInMap.get(next) == 0) {
                    zeroInQueue.add(next);
                }
            }
        }

        return result;
    }
}

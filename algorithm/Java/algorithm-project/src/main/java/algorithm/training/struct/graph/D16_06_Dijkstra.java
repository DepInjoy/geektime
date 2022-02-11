package algorithm.training.struct.graph;

import java.util.HashMap;
import java.util.HashSet;
import java.util.Map.Entry;

/**
 * @brief:          Dijkstra算法实现有权图的单源最短路径算法，有两种实现方式：
 *                      1. 直接扫描所有未收录顶点，适合于稠密图
 *                      2. 将收集到的最短路径放在小顶堆中，适合于稀疏图
 *                          1) 手写堆结构实现将小顶堆中元素值更新
 * */
public class D16_06_Dijkstra {
    /**
     * @brief:      在未扫描的节点中查找最短路径采用直接扫描的方法, 适合于稠密图
     *
     *              时间复杂度为O(|V|^2 + |E|)
     * */
    public static HashMap<Node, Integer> dijkstra1(Node from) {
        if (from == null) return null;

        HashMap<Node, Integer> minDistanceMap = new HashMap<>();
        // 已经访问过的节点
        HashSet<Node> selectedNodes = new HashSet<>();
        minDistanceMap.put(from, 0);
        // 收录节点中距离最小的节点
        Node minNode = from;
        while (minNode != null) {
            int distance = minDistanceMap.get(minNode);
            // 寻找以当前最小为边辐射出去的最小距离
            for (Edge edge : minNode.edges) {
                Node toNode = edge.to;
                if (!minDistanceMap.containsKey(toNode)) {
                    minDistanceMap.put(toNode, distance + edge.weight);
                } else {
                    // 取已经收录过的点和以当前点为跳点的最小距离
                    minDistanceMap.put(toNode, Math.min(minDistanceMap.get(toNode),
                            distance + edge.weight));
                }
            }
            selectedNodes.add(minNode);
            minNode = getMinDistanceFromUnvisitedNodes(selectedNodes, minDistanceMap);
        }

        return minDistanceMap;
    }

     // 时间复杂度为O(V)，V为节点个数
    public static Node getMinDistanceFromUnvisitedNodes(HashSet<Node> visitedNodes,HashMap<
            Node, Integer> minDistanceMap) {
        Node minNode = null;
        Integer minDis = Integer.MAX_VALUE;
        // 从没有访问过的节点中查找最小值
        for (Entry<Node, Integer> entry: minDistanceMap.entrySet()) {
            Node node = entry.getKey();
            Integer distance = entry.getValue();
            if (!visitedNodes.contains(node) && distance < minDis) {
                minNode = node;
                minDis = distance;
            }
        }
        return minNode;
    }

    /**
     *  将dist存在最小堆中，适合于稀疏图
     *  时间复杂度为O(|V|log|V|+|E|log|V|)=O(|E|log|V| )
     *
     * */
    public static HashMap<Node, Integer> dijkstra2(Node from, int size) {
        if (from == null) return null;

        NodeHeap prioNodeHeap = new NodeHeap(size);
        HashMap<Node, Integer> minDistanceMap = new HashMap<>();
        prioNodeHeap.addOrUpdateOrIgnore(from, 0);
        while (prioNodeHeap.isEmpty()) {
            NodeRecocord minNodeRecord = prioNodeHeap.pop();
            for (Edge edge : minNodeRecord.node.edges) {
                prioNodeHeap.addOrUpdateOrIgnore(edge.to, minNodeRecord.minDistance + edge.weight);
            }
            minDistanceMap.put(minNodeRecord.node, minNodeRecord.minDistance);
        }
        return minDistanceMap;
    }

    // 记录节点和从from到当前节点的最小距离
    public static class NodeRecocord {
        public Node node;
        public int minDistance;

        NodeRecocord(Node node, int minDistance) {
            this.node = node;
            this.minDistance = minDistance;
        }
    }

    static class  NodeHeap { // 实现小顶堆
        // 堆大小
        private int size;
        private Node[] nodes;
        // key:节点 value:节点在数组存储中的映射索引
        // 同时含有节点是否收集过的信息，已经被收集的节点的Index为-1
        private HashMap<Node, Integer> heapIndexMap;
        // key:节点 value:从from节点到当前节点的最小距离
        private  HashMap<Node, Integer> distanceMap;

        public NodeHeap(int size) {
            this.size = 0;
            nodes = new Node[size];
            heapIndexMap = new HashMap<>();
            distanceMap = new HashMap<>();
        }

        // 将堆中索引为i1和i2的两元素交换
        private void swap(int i1, int i2) {
            heapIndexMap.put(nodes[i1], i2);
            heapIndexMap.put(nodes[i2], i1);
            Node tmp = nodes[i1];
            nodes[i1] = nodes[i2];
            nodes[i2] = tmp;
        }

        // 节点是否已经在堆中, 且已经收集到从from到当前节点的最小距离
        private boolean inHeap(Node node) {
            return heapIndexMap.containsKey(node) && heapIndexMap.get(node) != -1;
        }

        // 节点是否已经被收集
        private boolean isEntered(Node node) {
            return heapIndexMap.containsKey(node);
        }

        // 修改堆中元素,向上调整
        private void insertHeapify(Node node, int index) {
            int parentIndex = (index - 1) / 2;
            // 当前节点比父节点小, 调整
            while (distanceMap.get(nodes[index]) < distanceMap.get(nodes[parentIndex])) {
                parentIndex = (index - 1) / 2;
                swap(index, parentIndex);
                index = parentIndex;
            }
        }

        public boolean isEmpty() {
            return size == 0;
        }

        // 弹出元素,向下调整
        private void heapify(int index, int size) {
            int leftChild = 2 * index + 1;
            while (leftChild < size) {
                // 存在右节点且右节点<左节点,最小节点为右节点，否则为左节点
                int smallest = (leftChild + 1 < size) && (distanceMap.get(nodes[leftChild+1])
                        < distanceMap.get(nodes[leftChild])) ? leftChild + 1 : leftChild;
                if (distanceMap.get(nodes[smallest]) < distanceMap.get(nodes[index])) {
                    swap(index, smallest);
                    index = smallest;
                    leftChild = 2 * index + 1;
                } else{
                    break;
                }
            }
        }

        public void addOrUpdateOrIgnore(Node node, int distance) {
            // 已经收集过的节点，更新数值
            if (inHeap(node)) {
                distanceMap.put(node, Math.min(distanceMap.get(node), distance));
                insertHeapify(node, heapIndexMap.get(node));
            }

            // 没有收集过的节点,添加到堆中
            if (!isEntered(node)) {
                nodes[size] = node;
                distanceMap.put(node, distance);
                heapIndexMap.put(node, size);
                insertHeapify(node, size++);
            }
        }

        public NodeRecocord pop() {
            NodeRecocord result = new NodeRecocord(nodes[0], distanceMap.get(nodes[0]));
            // 将头节点索引更新为-1表示该节点已经被收集
            heapIndexMap.put(nodes[0], -1);
            distanceMap.remove(nodes[0]);
            // 将头元素和最后一个元素交换，确保堆依然是完全二叉树,之后进行堆调整
            swap(0, size-1);
            nodes[size-1]=null; // 堆顶元素析构
            heapify(0, --size);
            return result;
        }
    }

    public static void main(String[] args) {
        System.out.println("单源最短路径Dijkstra");
    }
}

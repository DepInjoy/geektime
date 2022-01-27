package algorithm.training.struct.graph;

import java.util.HashSet;
import java.util.LinkedList;
import java.util.Queue;

/**
 * @brief:                          宽度优先遍历
 *
 *              实现思想: 从根节点开始入队，将从该节点开始可以访问到的节点都依次入队(
 *                      由于树可能存在环，因此需要记录已经访问过的节点，避免二次加入访问)
 *                      之后将队列中的元素一一出队执行所需要的操作
 *                      
 *              实现理解: 该实现相当于在当前位置横扫一下当前可以到达的所有节点，将其记录并进行一一访问
 */
public class D16_01_BFS {
    // 代表对遍历到的节点的处理，在该实现中是节点数值打印
    public static void handleCurNode(Node node) {
        System.out.println(node.value);
    }

    public static void BFS(Node root) {
        if(root == null) return;

        Queue<Node> queue = new LinkedList<Node>();
        HashSet<Node> visited = new HashSet<Node>();  // 已访问过的节点
        queue.add(root);
        visited.add(root);
        while (!queue.isEmpty()) {
            Node cur = queue.poll();
            handleCurNode(cur);
            for (Node next : cur.nexts) {
                if (visited.contains(next)) { // 节点没有被访问过
                    queue.add(next);
                    visited.add(root);
                }
            }
        }
    }
}

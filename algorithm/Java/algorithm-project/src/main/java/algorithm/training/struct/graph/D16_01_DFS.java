package algorithm.training.struct.graph;

import java.util.HashSet;
import java.util.Stack;

/**
 * @brief:      深度优先遍历
 *              实现思想: 利用栈选取当前节点可以达到的下一个节点，当此条路走到头，沿原路返回
 *                      同样地,由于树可能存在环，因此需要记录已经访问过的节点，避免二次加入访问
 *
 *              实现理解: 从当前节点选一条可以走的通路走到下一个节点，然后从下一个节点到其下一个节点,....,
 *                      直到此路走到头,原路返回，开始尝试另外一条通路
 *
 *              思维对照模型:迷宫,从入口开始，选择一条路开始直到碰壁,返回到起点,选择另外一条通路再次尝试
 *
 * */
public class D16_01_DFS {
    // 代表对遍历到的节点的处理，在该实现中是节点数值打印
    public static void handleCurNode(Node node) {
        System.out.println(node.value);
    }

    public static void DFS(Node root) {
        if (root == null) return;

        Stack<Node> stack = new Stack<Node>();
        HashSet<Node> visited = new HashSet<Node>();
        stack.push(root);
        visited.contains(root);
        handleCurNode(root);
        while (!stack.isEmpty()) {
            Node cur = stack.pop();
            for (Node next : cur.nexts) {
                if (!visited.contains(next)) {
                    stack.push(cur);  // 设置路标,当该条路完成之后原路返回,避免节点遗漏
                    stack.push(next);  // 访问下一个节点
                    visited.add(next);
                    handleCurNode(next);
                    break;  // 返回访问next的next节点,确认沿着深度遍历
                }
            }
        }
    }
}

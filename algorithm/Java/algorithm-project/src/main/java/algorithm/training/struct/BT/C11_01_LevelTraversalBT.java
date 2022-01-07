package algorithm.training.struct.BT;

import java.util.LinkedList;

/**
 * @brief:      二叉树按层遍历
 *              实现思路:
 *                  1. 采用队列实现,从根节点开始,将元素从尾部加入队列
 *                     依次取出元素时，将非空的left和right加入队列
 * */
public class C11_01_LevelTraversalBT {
    public static void handleCurNode(TreeNode curNode) {
        System.out.print(curNode.val + " ");
    }

    public static void levelTraversal(TreeNode curNode) {
        if (curNode == null) return;

        LinkedList<TreeNode> queue = new LinkedList<TreeNode>();
        queue.add(curNode);
        while (!queue.isEmpty()) {
            TreeNode node = queue.poll();
            handleCurNode(node);
            if (node.left != null) {
                queue.add(node.left);
            }
            if (node.right != null) {
                queue.add(node.right);
            }
        }
    }

    public static void main(String[] args) {
        TreeNode head = new TreeNode(1);
        head.left = new TreeNode(2);
        head.right = new TreeNode(3);
        head.left.left = new TreeNode(4);
        head.left.right = new TreeNode(5);
        head.right.left = new TreeNode(6);
        head.right.right = new TreeNode(7);

        System.out.println("levelTraversal: ");
        levelTraversal(head);
        System.out.println("\n");
    }
}

package algorithm.training.struct.BT;

import java.util.Stack;

public class UnrecursiveTraversalBT_2_Star {
    public static void handleCurNode(TreeNode curNode) {
        System.out.print(curNode.val + " ");
    }

    public static void PreOrder(TreeNode root) {
        if (root == null) return;

        TreeNode curNode = root;
        Stack<TreeNode> stack = new Stack<TreeNode>();
        while (curNode != null || !stack.isEmpty()) {
            while (curNode != null) { // 一直向左并将沿途节点压入堆栈
                stack.push(curNode); // 第一次访问节点
                handleCurNode(curNode); // 处理节点
                curNode = curNode.left;
            }

            if (!stack.isEmpty()) {
                curNode = stack.peek();
                stack.pop(); // 第二次访问节点
                curNode = curNode.right; // 转向右子树
            }
        }
    }

    public static void InOrder(TreeNode root) {
        if (root == null) return;

        TreeNode curNode = root;
        Stack<TreeNode> stack = new Stack<TreeNode>();
        while (curNode != null || !stack.isEmpty()) {
            while (curNode != null) {
                stack.push(curNode);
                curNode = curNode.left;
            }

            if (!stack.isEmpty()) {
                curNode = stack.peek();
                stack.pop();
                handleCurNode(curNode);
                curNode = curNode.right;
            }
        }
    }

    public static void PostOrder(TreeNode root) {
        if (root == null) return;

        TreeNode curNode = root;
        TreeNode lastNode = null; // 上一次访问的节点
        Stack<TreeNode> stack = new Stack<TreeNode>();
        while (curNode != null || !stack.isEmpty()) {
            while (curNode != null) {
                stack.push(curNode);
                curNode = curNode.left;
            }

            curNode = stack.peek(); // 出栈前，将其视作实际的根节点
            // 如果不存在右子树或右子树已经访问，那么访问根节点,出栈，并且不需要转向
            if (curNode.right == null || curNode.right == lastNode) {
                handleCurNode(curNode);
                stack.pop();
                // 记录上一个已经访问过的节点，用于判断访问根节点之前，右子树书否已经访问过了
                lastNode = curNode;
                // 表示不需要转向，继续出栈(根节点处理完成，接着左，左已入栈，则继续出栈)
                curNode = null;
            } else {
                curNode = curNode.right; // 转向
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

        System.out.println("Preorder: ");
        PreOrder(head);
        System.out.println("\n");

        System.out.println("Inorder: ");
        InOrder(head);
        System.out.println("\n");

        System.out.println("Postorder: ");
        PostOrder(head);
        System.out.println("\n");
    }
}

package algorithm.training.struct.BT;

/**
 * @brief:      递归地实现二叉树的前序先序/中序/后序遍历
 *              对于二叉树的遍历是递归序加工的结果，其中
 *                  1. 先序遍历:第一次到达节点执行对节点的操作(例如打印)
 *                  2. 中序遍历:第二次到达节点执行对节点的操作(例如打印)
 *                  3. 后序遍历:第三次到达节点执行对节点的操作(例如打印)
 * */

public class C10_02_RecursiveTraversalBT {
    public static void handleCurNode(TreeNode curNode) {
        System.out.print(curNode.val + " ");
    }

    public static void Preorder(TreeNode head) {
        if (head == null) return;

        handleCurNode(head);
        Preorder(head.left);
        Preorder(head.right);
    }

    public static void Inorder(TreeNode head) {
        if (head == null) return;

        Inorder(head.left);
        handleCurNode(head);
        Inorder(head.right);
    }

    public static void Postorder(TreeNode head) {
        if (head == null) return;

        Postorder(head.left);
        Postorder(head.right);
        handleCurNode(head);
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
        Preorder(head);
        System.out.println("\n");

        System.out.println("Inorder: ");
        Inorder(head);
        System.out.println("\n");

        System.out.println("Postorder: ");
        Postorder(head);
        System.out.println("\n");
    }
}

package algorithm.training.struct.BT;

import java.util.Stack;

public class C10_03_UnRecursiveTraversalBT {
    public static void handleCurNode(TreeNode curNode) {
        System.out.print(curNode.val + " ");
    }

    /**
     * @brief:      非递归地实现二叉树的先序遍历
     *              实现思路:
     *                  1. 节点弹出便执行对节点的操作
     *                  2. 如果右树不为空，则右树入栈
     *                  3. 如果左树不为空，则左树入栈
     * */
    public static void Preorder(TreeNode head) {
        if (head == null) return;

        Stack<TreeNode> stack = new Stack<TreeNode>();
        stack.push(head);
        while (!stack.isEmpty()) {
            TreeNode curNode = stack.pop();
            handleCurNode(curNode);
            if (curNode.right != null) {
                stack.push(curNode.right);
            }
            if (curNode.left != null) {
                stack.push(curNode.left);
            }
        }
    }

    /**
     * @brief:      非递归地实现二叉树中序遍历
     *              实现思路：
     *                  1. 先将左边的整个树入栈
     *                  2. 如果为空，无法继续处理，执行对节点的操作
     *                  3. 继续对右边的树进行步骤1的处理
     * */
    public static void Inorder(TreeNode head) {
        if (head == null) return;

        Stack<TreeNode> stack  = new Stack<TreeNode>();
        TreeNode curNode = head;
        while (!stack.isEmpty() || curNode != null) {
            if (curNode != null) {
                stack.push(curNode);
                curNode = curNode.left;
            } else {
                curNode = stack.pop();
                handleCurNode(curNode);
                curNode = curNode.right;
            }
        }
    }

    /**
     * @brief:      非递归地实现二叉树后序遍历
     *              实现思想:
     *                  先序遍历顺序是根左右，后序遍历是左右跟，如果将先序遍历进行一下调整可以得到跟右左，得到后序遍历的逆序
     *                  采用栈结构，改造前序遍历，进栈，在出栈时对节点进行处理便可以得到后序遍历的结果
     *              实现思路:
     *                  1. 改进先序遍历，首先将入栈顺序调整为先左后右，对节点的处理调整为入栈
     *                  2. 将数据从用于结果暂存的栈中弹出，此时便是后序遍历的结果，对节点执行相关的相关的操作(例如打印)
     * */
    public static void Postorder1(TreeNode head) {
        if (head == null) return;

        Stack<TreeNode> stack = new Stack<TreeNode>();
        Stack<TreeNode> resStack = new Stack<TreeNode>();
        stack.push(head);
        while (!stack.isEmpty()) {
            TreeNode curNode = stack.pop();
            resStack.push(curNode);
            if (curNode.left != null) {
                stack.push(curNode.left);
            }
            if (curNode.right != null) {
                stack.push(curNode.right);
            }
        }
        while (!resStack.isEmpty()) {
            TreeNode node = resStack.pop();
            handleCurNode(node);
        }
    }

    /**
     * @brief:      非递归地实现二叉树后序遍历
     *              实现思想:
     *              实现思路:
     *                  1. 左树未处理,则处理左树
     *                  2. 右树未处理,则处理右树
     *                  3. 左右树都处理完成，则执行节点操作
     * */
    public static void Postorder2(TreeNode head) {
        if (head == null) return;

        Stack<TreeNode> stack = new Stack<TreeNode>();
        TreeNode curNode = null;
        stack.push(head);
        while (!stack.isEmpty()) {
            curNode = stack.peek();
            if (curNode.left != null && head != curNode.right && head != curNode.left) {
                stack.push(curNode.left);
            } else if (curNode.right != null && head != curNode.right) {
                stack.push(curNode.right);
            } else {
                handleCurNode(stack.pop());
                head = curNode;
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
        Preorder(head);
        System.out.println("\n");

        System.out.println("Inorder: ");
        Inorder(head);
        System.out.println("\n");

        System.out.println("Postorder1: ");
        Postorder1(head);
        System.out.println("\n");

        System.out.println("Postorder2: ");
        Postorder2(head);
        System.out.println("\n");
    }
}

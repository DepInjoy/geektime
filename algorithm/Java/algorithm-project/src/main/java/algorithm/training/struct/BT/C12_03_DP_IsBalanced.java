package algorithm.training.struct.BT;

/**
 * @brief:      给定一棵二叉树的头节点head，返回这颗二叉树是不是平衡二叉树
 *              需要满足条件：
 *                  1. 左/右子树是平衡二叉树
 *                  2. 左右子树的高度差小于1
 *              因此可以根据递归套路，构造如下信息：
 *                  isBalanced  是否是平衡二叉树
 *                  height      树的高度
 *              依次对左右子树进行信息填充
 * */
public class C12_03_DP_IsBalanced {
    static class DPNodeInfo{
        boolean isBalanced;
        Integer height;
        DPNodeInfo(boolean isBalanced, Integer height) {
            this.isBalanced = isBalanced;
            this.height = height;
        }
    }

    public static boolean IsBalanced(TreeNode head) {
        return process(head).isBalanced;
    }

    public static DPNodeInfo process(TreeNode head) {
        if (head == null) return new DPNodeInfo(true, 0);
        DPNodeInfo leftInfo = process(head.left);
        DPNodeInfo rightInfo = process(head.right);
        boolean isBalanced = false;
        if (leftInfo.isBalanced && rightInfo.isBalanced &&
                Math.abs(leftInfo.height - rightInfo.height) <= 1) {
            isBalanced = true;
        }
        Integer height = Math.max(leftInfo.height, rightInfo.height) + 1;
        return new DPNodeInfo(isBalanced, height);
    }

    public static void main(String[] args) {
        // Case1: 空树是平衡二叉树
        TreeNode head0 = null;
        assert IsBalanced(head0) == true;

        /* Case2:构造一颗平衡二叉树
                            1
                    2               3
                4
            是平衡二叉树
         */
        TreeNode head1 = new TreeNode(1);
        head1.left = new TreeNode(2);
        head1.right = new TreeNode(3);
        head1.left.left = new TreeNode(4);
        assert IsBalanced(head1) == true;

        /* Case3: 构造一颗非平衡二叉树
                            1
                    2               3
                4
            5
            不是是平衡二叉树
         */
        TreeNode head2 = new TreeNode(1);
        head2.left = new TreeNode(2);
        head2.right = new TreeNode(3);
        head2.left.left = new TreeNode(4);
        head2.left.left.left = new TreeNode(5);
        assert IsBalanced(head2) == false;
    }
}

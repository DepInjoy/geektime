package algorithm.training.struct.BT;

import sun.reflect.generics.tree.Tree;

/**
 * @brief:  给定一棵二叉树的头节点head，返回这颗二叉树是不是搜索二叉树
 *          需要满足的条件：
 *              1. 含root，左子树的最大值< root.val < 右子树的最小值
 *              2. 不含root，左/右子树均为BST
 *          依此需要填充的信息有：
 *              isBST:      左右子树是否是搜索二叉树
 *              maxVal:     子树最大值
 *              minVal:     子树最小值
 * */
public class C12_02_DP_IsBST {
    static class DPNodeInfo {
        public boolean isBST;
        public Integer maxVal;
        public Integer minVal;

        DPNodeInfo(boolean isBST, Integer maxVal, Integer minVal) {
            this.isBST = isBST;
            this.maxVal = maxVal;
            this.minVal = minVal;
        }
    }

    public static boolean isBST(TreeNode head) {
        if (head == null) {
            return true;
        } else {
            return process(head).isBST;
        }
    }

    public static DPNodeInfo process(TreeNode head) {
        if (head == null) return null;

        DPNodeInfo leftNodeInfo = process(head.left);
        DPNodeInfo rightNodeInfo = process(head.right);
        Integer maxVal = head.val;
        Integer minVal = head.val;
        boolean isBST = false;
        if (leftNodeInfo != null) maxVal = Math.max(maxVal, leftNodeInfo.maxVal);
        if (rightNodeInfo != null) minVal = Math.min(minVal, rightNodeInfo.minVal);
        if ((leftNodeInfo != null ?  leftNodeInfo.isBST : true) && // 1. 左子树是BST
                (rightNodeInfo != null ? rightNodeInfo.isBST : true) && // 2. 右子树是BST
                // 3. 存在左右子树，则左子树的最大值< root.val < 右子树的最小值
                (leftNodeInfo == null || (leftNodeInfo != null && leftNodeInfo.maxVal < head.val)) &&
                    (rightNodeInfo == null || (rightNodeInfo != null && head.val < rightNodeInfo.minVal))) {
            isBST = true;
        }
        return new DPNodeInfo(isBST, maxVal, minVal);
    }

    public static void main(String[] args) {
        // Case1: 容错测试,空树也是BST
        TreeNode head0 = null;
        assert isBST(head0) == true;

        /* Case2: 构造如下树：
         *               2
         *           1       3
         *  是BST
         */
        TreeNode head1 = new TreeNode(2);
        head1.left = new TreeNode(1);
        head1.right = new TreeNode(3);
        assert isBST(head1) == true;

        /*  Case3: 构造如下树：
        *           1
        *       2       3
        * 不是BST
        */
        TreeNode head2 = new TreeNode(1);
        head2.left = new TreeNode(2);
        head2.right = new TreeNode(3);
        assert isBST(head2) == false;

        /*
        * Case4: 构造树：
        *                 4
        *              /      \
        *           2           6
        *         /     \    /
        *       1       3   5
        *   是BST
        * */
        TreeNode head3 = new TreeNode(4);
        head3.left = new TreeNode(2);
        head3.right = new TreeNode(6);
        head3.left.left = new TreeNode(1);
        head3.left.right = new TreeNode(3);
        head3.right.left = new TreeNode(5);
        assert isBST(head3) == true;
    }
}

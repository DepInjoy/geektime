package algorithm.training.struct.BT;

/**
 * @brief:      给定一棵二叉树的头节点head，返回这颗二叉树是不是满二叉树
 *              根据满二叉树的定义，满二叉树需要满足：
 *                  1.第K层的二叉树，总节点数为2^k-1个
 *              那么，需要收集如下信息：
 *                  height:     树的高度
 *                  numNodes:   当前层节点数
 * */
public class C12_04_DP_IsFull {
    static class DPNodeInfo {
        public Integer height;
        public Integer numNodes;
        DPNodeInfo(Integer height, Integer numNodes) {
            this.height = height;
            this.numNodes = numNodes;
        }
    }

    public static boolean IsFull(TreeNode head) {
        DPNodeInfo info = process(head);
        return (1 << info.height) - 1 == info.numNodes;
    }

    public static DPNodeInfo process(TreeNode root) {
        if (root == null) return new DPNodeInfo(0, 0);

        DPNodeInfo leftNodeInfo = process(root.left);
        DPNodeInfo rightNodeInfo = process(root.right);
        Integer height = Math.max(leftNodeInfo.height, rightNodeInfo.height) + 1;
        Integer numNodes = leftNodeInfo.numNodes + rightNodeInfo.numNodes + 1;
        return new DPNodeInfo(height, numNodes);
    }

    public static void main(String[] args) {
        // Case1: 容错检查，空树是一颗满二叉树
        TreeNode head0 = null;
        assert IsFull(head0) == true;

        /*
        * Case2: 构造如下二叉树：
        *               2
        *           1       3
        *   是满二叉树
        * */
        TreeNode head1 = new TreeNode(1);
        head1.left = new TreeNode(2);
        head1.right = new TreeNode(3);
        assert IsFull(head1) == true;

        /*
         * Case3: 构造如下二叉树：
         *               2
         *           1       3
         *      4
         *   不是满二叉树
         * */
        TreeNode head2 = new TreeNode(1);
        head2.left = new TreeNode(2);
        head2.right = new TreeNode(3);
        head2.left.left = new TreeNode(4);
        assert IsFull(head2) == false;
    }
}

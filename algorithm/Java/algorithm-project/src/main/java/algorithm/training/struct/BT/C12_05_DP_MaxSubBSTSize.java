package algorithm.training.struct.BT;

/**
 * @brief:  给定一棵二叉树的头节点head，返回这颗二叉树中最大的二叉搜索子树的大小
 *
 *
 * */
public class C12_05_DP_MaxSubBSTSize {
    static class DPNodeInfo {
        Integer     maxBSTSize;
        DPNodeInfo(Integer maxBSTSize) {
            this.maxBSTSize = maxBSTSize;
        }
    }

    public static DPNodeInfo process(TreeNode root) {
        if (root == null) return new DPNodeInfo(0);
        return new DPNodeInfo(0);
    }

    public static void main(String[] args) {

    }
}

package algorithm.training.struct.BT;

import java.util.HashMap;
import java.util.LinkedList;

/**
 * @brief:      统计二叉树的最大宽度
 *              关注重点:
 *                  解决该问题的重点在于获取每一层的节点数
 *              实现思路:
 *                  1. 将节点和树的深度构成一个map,计算每层节点数并取最大值
 *                  2. 记录当前节点、当前层最后一个节点以及下一层最后一个节点
 *                     当前节点和当前层最后一个节点相遇，就可以获取到当前层的节点个数
 * */
public class C11_05_BTMaxWidth {
    public static Integer maxWidthWithMap(TreeNode root) {
        if (root == null) return 0;

        HashMap<TreeNode, Integer> nodeLevelMap = new HashMap<TreeNode, Integer>();
        LinkedList<TreeNode> queue = new LinkedList<TreeNode>();
        queue.add(root);
        nodeLevelMap.put(root, 1);
        Integer maxWidth = 0;
        Integer curLevel = 1;   // 当前层数
        Integer curLevelNodes = 0;  // 当前层的节点数
        while (!queue.isEmpty()) {
            TreeNode curNode = queue.poll();
            Integer curNodeLevel = nodeLevelMap.get(curNode); // 当前节点所在层数
            if (curNode.left != null) {
                queue.add(curNode.left);
                nodeLevelMap.put(curNode.left, curNodeLevel + 1);
            }
            if (curNode.right != null) {
                queue.add(curNode.right);
                nodeLevelMap.put(curNode.right, curNodeLevel + 1);
            }
            if (curLevel == curNodeLevel) {
                curLevelNodes++;
            } else { // 开始下一层
                curLevel++;
                maxWidth = Math.max(maxWidth, curLevelNodes);
                curLevelNodes = 1; // 开始新层,重置下一层的节点数
            }
        }
        maxWidth = Math.max(maxWidth, curLevelNodes);
        return maxWidth;
    }

    public static Integer maxWidth(TreeNode root) {
        if (root == null) return 0;

        LinkedList<TreeNode> queue = new LinkedList<TreeNode>();
        queue.add(root);
        TreeNode curEndNode = root;
        TreeNode nextLevelEndNode = root;
        Integer maxWidth = 0;
        Integer curLevelNodes = 0;
        while (!queue.isEmpty()) {
            TreeNode curNode = queue.poll();
            if (curNode.left != null) {
                queue.add(curNode.left);
                nextLevelEndNode = curNode.left;
            }
            if (curNode.right != null) {
                queue.add(curNode.right);
                nextLevelEndNode = curNode.right;
            }
            if (curNode != curEndNode) {
                curLevelNodes++;
            } else {
                curEndNode = nextLevelEndNode;
                maxWidth = Math.max(maxWidth, curLevelNodes);
                curLevelNodes = 1;
            }
        }
        maxWidth = Math.max(maxWidth, curLevelNodes);
        return maxWidth;
    }

    public static void main(String[] args) {
        TreeNode head = new TreeNode(1);
        head.left = new TreeNode(2);
        head.right = new TreeNode(3);
        head.left.left = new TreeNode(4);
        head.left.right = new TreeNode(5);
        head.right.left = new TreeNode(6);
        head.right.right = new TreeNode(7);
        assert maxWidthWithMap(head) == 4;
        assert maxWidth(head) == 4;

        // 容错测试
        TreeNode head0 = null;
        assert maxWidthWithMap(head0) == 0;
        assert maxWidth(head0) == 0;

        TreeNode head1 = new TreeNode(1);
        assert maxWidthWithMap(head1) == 1;
        assert maxWidth(head1) == 1;
    }
}

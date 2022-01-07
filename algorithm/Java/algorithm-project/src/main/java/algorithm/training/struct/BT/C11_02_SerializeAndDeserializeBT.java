package algorithm.training.struct.BT;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.Queue;
import java.util.Stack;

/**
 * @brief:
 *      二叉树可以通过先序、后序或者按层遍历的方式序列化和反序列化,
 *      但是，二叉树无法通过中序遍历的方式实现序列化和反序列化
 *      因为不同的两棵树，可能得到同样的中序序列，即便补了空位置也可能一样。
 *      比如如下两棵树
 *         __2
 *        /
 *       1
 *       和
 *       1__
 *          \
 *           2
 *      补足空位置的中序遍历结果都是{ null, 1, null, 2, null}
 */
public class C11_02_SerializeAndDeserializeBT {
    /**
     * @brief:      1. 前序遍历实现序列化和反序列化
     *                  1.1 PreorderSerialize(TreeNode root)实现前序遍历的序列化
     *                  1.2 PreorderDerialize(Queue<String> request)实现前序遍历的反序列化
     */
    public static Queue<String> PreorderSerialize(TreeNode root) {
        Queue<String> res = new LinkedList<String>();
        PreorderSerialize(root, res);
        return res;
    }

    public static void PreorderSerialize(TreeNode root, Queue<String> ans) {
        if (root == null) {
            ans.add(null);
        } else {
            ans.add(String.valueOf(root.val));
            PreorderSerialize(root.left, ans);
            PreorderSerialize(root.right, ans);
        }
    }

    public static TreeNode PreorderDerialize(Queue<String> request) {
        String value = request.poll();
        if (value == null) return null;

        TreeNode root = new TreeNode(Integer.valueOf(value));
        root.left = PreorderDerialize(request);
        root.right = PreorderDerialize(request);
        return root;
    }

    /**
     * @brief:      2. 后序遍历实现序列化和反序列化
     *                  1.1 PostorderSerialize(TreeNode root)实现后序遍历的序列化
     *                  1.2 PostorderDerialize(Queue<String> request)实现后序遍历的反序列化
     */
    public static Queue<String> PostorderSerialize(TreeNode root) {
        Queue<String> res = new LinkedList<String>();
        PostorderSerialize(root, res);
        return res;
    }

    public static void PostorderSerialize(TreeNode root, Queue<String> response) {
        if (root == null) {
            response.add(null);
        } else {
            PostorderSerialize(root.left, response);
            PostorderSerialize(root.right, response);
            response.add(String.valueOf(root.val));
        }
    }

    // 后序遍历的左右中转化为中右左
    public static Stack<String> transfer2Stack(Queue<String> request) {
        if (request == null || request.size() == 0) return null;

        Stack<String> stack = new Stack<String>();
        while (!request.isEmpty()) {
            stack.push(request.poll());
        }
        return stack;
    }

    public static TreeNode PostorderDerialize(Stack<String> request) {
        if (request == null) return null;

        String value = request.pop();
        if (value == null) return null;

        TreeNode node = new TreeNode(Integer.valueOf(value));
        node.right = PostorderDerialize(request);
        node.left = PostorderDerialize(request);
        return node;
    }

    public static TreeNode PostorderDerialize(Queue<String> request) {
        Stack<String> stack = transfer2Stack(request);
        return PostorderDerialize(stack);
    }

    /**
     * @brief:      3. 按层遍历实现序列化和反序列化
     *                  1.1 LevelorderSerialize(TreeNode root)实现按层遍历的序列化
     *                  1.2 LevelorderDerialize(Queue<String> request)实现按层遍历的反序列化
     */
    public static Queue<String> LevelorderSerialize(TreeNode root) {
        if (root == null) return null;

        Queue<String> res = new LinkedList<String>();
        // 辅助记录每一层非空的节点
        Queue<TreeNode> queue = new LinkedList<TreeNode>();
        queue.add(root);
        res.add(String.valueOf(root.val));
        while (!queue.isEmpty()) {
            TreeNode curNode = queue.poll();
            if (curNode.left != null) {
                queue.add(curNode.left);
                res.add(String.valueOf(curNode.left.val));
            } else {
                res.add(null);
            }
            if (curNode.right != null) {
                queue.add(curNode.right);
                res.add(String.valueOf(curNode.right.val));
            } else {
                res.add(null);
            }
        }
        return res;
    }

    public static TreeNode generateNode(String value) {
        if (value == null) return null;
        return new TreeNode(Integer.valueOf(value));
    }

    public static TreeNode LevelorderDerialize(Queue<String> request) {
        if (request == null || request.size() == 0) return null;

        TreeNode node = generateNode(request.poll());
        LinkedList<TreeNode> queue = new LinkedList<TreeNode>();
        queue.add(node);
        while (!queue.isEmpty()) {
            TreeNode curNode = queue.poll();
            curNode.left = generateNode(request.poll());
            curNode.right = generateNode(request.poll());
            if (curNode.left != null) queue.add(curNode.left);
            if (curNode.right != null) queue.add(curNode.right);
        }
        return node;
    }

    // Just for UT following
    public static boolean isSameTree(TreeNode left, TreeNode right) {
        if (left == null && right == null) return true;
        if (left == null && right != null) return false;
        if (left != null && right == null) return false;
        if (left.val != right.val) return false;
        return isSameTree(left.left, right.left) && isSameTree(left.right, right.right);
    }

    public static void ut(TreeNode head) {
        Queue<String> pre = PreorderSerialize(head);
        TreeNode preNode = PreorderDerialize(pre);
        assert isSameTree(head, preNode);

        Queue<String> post = PostorderSerialize(head);
        TreeNode postNode = PostorderDerialize(post);
        assert isSameTree(head, postNode);

        Queue<String> level = LevelorderSerialize(head);
        TreeNode levelNode = LevelorderDerialize(level);
        assert isSameTree(head, postNode);
    }

    public static void main(String[] args) {
        // 容错测试
        ut(null);

        TreeNode head = new TreeNode(1);
        head.left = new TreeNode(2);
        head.right = new TreeNode(3);
        head.left.left = new TreeNode(4);
        head.left.right = new TreeNode(5);
        head.right.left = new TreeNode(6);
        head.right.right = new TreeNode(7);
        ut(head);

        TreeNode head2 = new TreeNode(1);
        head.left = new TreeNode(2);
        head.right = null;
        head.left.left = new TreeNode(4);
        head.left.right = null;
        ut(head2);
    }
}

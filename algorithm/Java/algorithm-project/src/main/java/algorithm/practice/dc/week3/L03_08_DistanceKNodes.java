package algorithm.practice.dc.week3;

import java.util.*;

/**
 * @brief   给定三个参数：二叉树的头节点head，树上某个节点target，正数K
 *          从target开始，可以向上走或者向下走
 *          返回与target的距离是K的所有节点
 *
 */
public class L03_08_DistanceKNodes {
    public static class Node {
        public int value;
        public Node left;
        public Node right;

        public Node(int v) {
            value = v;
        }
    }

    public static List<Node> distanceKNodes(Node root, Node target, int K) {
        if (root == null || target == null) return null;

        HashMap<Node, Node> parentChildMap = new HashMap<>();
        List<Node> ans = new ArrayList<>();
        Queue<Node> queue = new LinkedList<>();
        HashSet<Node> visited = new HashSet<>();
        queue.add(target);
        createParentMap(root, parentChildMap);
        int curLevel = 0;
        while (!queue.isEmpty()) {
            int curLevelSize = queue.size();
            while (curLevelSize-- > 0) {
                Node curNode = queue.poll();
                if (curLevel == K) ans.add(curNode);

                if (curNode.left != null && !visited.contains(curNode.left)) {
                    queue.add(root.left);
                    visited.add(root.left);
                }

                if (curNode.right != null && !visited.contains(curNode.right)) {
                    queue.add(curNode.right);
                    visited.add(root.left);
                }

                Node parentNode = parentChildMap.get(curNode);
                if (!visited.contains(parentNode)) {
                    queue.add(parentNode);
                    visited.add(parentNode);
                }

                if (curLevel > K) break;
            }
            curLevel++;
        }
        return ans;
    }

    public static void createParentChildMap(Node root, HashMap<Node, Node> parentChildMap) {
        if (root == null) return;

        if (root.left != null) {
            parentChildMap.put(root.left, root);
            createParentMap(root.left, parentChildMap);
        }

        if (root.right != null) {
            parentChildMap.put(root.right, root);
            createParentMap(root.right, parentChildMap);
        }
    }

    public static List<Node> distanceKNodes2(Node root, Node target, int K) {
        if (root == null || target == null) return null;

        HashMap<Node, Node> parentMap = new HashMap<>();
        List<Node> ans = new ArrayList<>();
        Queue<Node> queue = new LinkedList<>();
        HashSet<Node> visited = new HashSet<>();
        queue.add(target);
        visited.add(target);
        createParentMap(root, parentMap);
        int curLevel = 0;
        while (!queue.isEmpty()) {
            int curLevelSize = queue.size();
            while (curLevelSize-- > 0) {
                Node curNode = queue.poll();
                if (curLevel == K) {
                    ans.add(curNode);
                }

                if (curNode.left != null && !visited.contains(curNode.left)) {
                    queue.add(curNode.left);
                    visited.add(curNode.left);
                }

                if (curNode.right != null && !visited.contains(curNode.right)) {
                    queue.add(curNode.right);
                    visited.add(curNode.right);
                }

                Node curParentNode = parentMap.get(curNode);
                if (curParentNode != null && !visited.contains(curParentNode)) {
                    queue.add(curParentNode);
                    visited.add(curParentNode);
                }
            }
            curLevel++;
            if (curLevel > K) break;
        }
        return ans;
    }

    public static void createParentMap(Node root, HashMap<Node, Node> parentMap) {
        if (root == null) return;

        if (root.left != null) {
            parentMap.put(root.left, root);
            createParentMap(root.left, parentMap);
        }

        if (root.right != null) {
            parentMap.put(root.right, root);
            createParentMap(root.right, parentMap);
        }
    }
/**
               3
           /       \
        5           1
      /    \     /     \
    6       2   0       8
         /    \
        7       4
 */
    public static void main(String[] args) {
        Node n0 = new Node(0);
        Node n1 = new Node(1);
        Node n2 = new Node(2);
        Node n3 = new Node(3);
        Node n4 = new Node(4);
        Node n5 = new Node(5);
        Node n6 = new Node(6);
        Node n7 = new Node(7);
        Node n8 = new Node(8);

        n3.left = n5;
        n3.right = n1;
        n5.left = n6;
        n5.right = n2;
        n1.left = n0;
        n1.right = n8;
        n2.left = n7;
        n2.right = n4;

        Node root = n3;
        Node target = n5;
        int K = 2;

        List<Node> ans = distanceKNodes2(root, target, K);
        for (Node o1 : ans) {
            System.out.println(o1.value);
        }
    }
}

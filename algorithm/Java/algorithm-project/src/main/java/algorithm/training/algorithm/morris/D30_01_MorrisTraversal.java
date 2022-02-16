package algorithm.training.algorithm.morris;

/**
 * @brief:          Morris遍历二叉树
 *                  时间复杂度O(N)，额外空间复杂度O(1)
 *                  实现方式：
 *                      假设来到当前节点cur，开始时cur来到头节点位置
 *                          1. 如果cur没有左孩子，cur向右移动(cur = cur.right)
 *                          2. 如果cur有左孩子，找到左子树上最右的节点mostRight：
 * 	                            a.如果mostRight的右指针指向空，让其指向cur，然后cur向左移动(cur = cur.left)
 * 	                            b.如果mostRight的右指针指向cur，让其指向null, 然后cur向右移动(cur = cur.right)
 *                          3. cur为空时遍历停止
 *
 *                  核心思想:
 *                      通过利用原树中大量空闲指针的方式，达到节省空间的目的,它建立一种机制:
 *                      对于没有左子树的节点只到达一次, 对于有左子树的节点会到达两次, morris遍历时间复杂度依然是O(N)
 * */
public class D30_01_MorrisTraversal {
    public static class Node {
        public int value;
        public Node left;
        public Node right;

        public Node(int data) {
            value = data;
        }
    }

    public static void morris(Node head) {
        if (head == null) return;

        Node cur = head;
        Node mostRight = null;
        while (cur != null) {
            mostRight = cur.left;
            if (mostRight != null) { // 有左孩子
                // 寻找左子树的最右节点
                while (mostRight.right != null && mostRight.right != cur) {
                    mostRight = mostRight.right;
                }
                if (mostRight.right == null) {
                    mostRight.right = cur;
                    cur = cur.left;
                    continue;
                } else {
                    mostRight.right = null;
                }
            }
            // 没有左孩子或mostRight为空, cur向右移动
            cur = cur.right;
        }
    }

    public static void handleCurNode(Node node) {
        System.out.print(node.value + " ");
    }

    public static void process(Node root) {
        if (root == null) return;
        // 第一次访问该节点 PreOrder
        process(root.left);
        // 第二次访问该节点 InOrder
        process(root.left);
        // 第三次访问该节点 PostOrder
    }

    // 利用Morris序实现先序遍历
    // 无左子树，则处理当前节点，有左子树则第一次来到则打印
    public static void morrisPre(Node head) {
        if (head == null) return;

        Node cur = head;
        Node mostRight = null;
        while (cur != null) {
            mostRight = cur.left;
            if (mostRight != null) { // 有左孩子
                // 寻找左子树的最右节点
                while (mostRight.right != null && mostRight.right != cur) {
                    mostRight = mostRight.right;
                }
                if (mostRight.right == null) {
                    handleCurNode(cur);
                    mostRight.right = cur;
                    cur = cur.left;
                    continue;
                } else {
                    mostRight.right = null;
                }
            } else {
                handleCurNode(cur);
            }
            cur = cur.right;
        }
        System.out.println();
    }


    // 将Morris序转化为中序
    // 无左树,则处理当前节点, 有左子树则第二次来到处理当前节点
    public static void morrisIn(Node head) {
        if (head == null) return;

        Node cur = head;
        Node mostRight = null;
        while (cur != null) {
            mostRight = cur.left;
            if (mostRight != null) { // 有左孩子
                // 寻找左子树的最右节点
                while (mostRight.right != null && mostRight.right != cur) {
                    mostRight = mostRight.right;
                }
                if (mostRight.right == null) {
                    mostRight.right = cur;
                    cur = cur.left;
                    continue;
                } else {
                    mostRight.right = null;
                }
            }
            handleCurNode(cur);
            cur = cur.right;
        }
        System.out.println();
    }

    // 将Morris序转化为后序
    // 第二次来到，逆序打印右边界
    public static void morrisPost(Node head) {
        if (head == null) return;

        Node cur = head;
        Node mostRight = null;
        while (cur != null) {
            mostRight = cur.left;
            if (mostRight != null) { // 有左孩子
                // 寻找左子树的最右节点
                while (mostRight.right != null && mostRight.right != cur) {
                    mostRight = mostRight.right;
                }
                if (mostRight.right == null) {
                    mostRight.right = cur;
                    cur = cur.left;
                    continue;
                } else {
                    mostRight.right = null;
                    printEdge(cur.left);
                }
            }
            cur = cur.right;
        }
        printEdge(head);
        System.out.println();
    }

    public static void printEdge(Node node) {
        Node tail = reverseEdge(node);
        Node cur = tail;
        while (cur != null) {
            handleCurNode(cur);
            cur = cur.right;
        }
        // 恢复成原序列
        reverseEdge(tail);
    }

    public static Node reverseEdge(Node from) {
        Node pre = null;
        Node next = null;
        while (from != null) {
            next = from.right;
            from.right = pre;
            pre = from;
            from = next;
        }
        return pre;
    }

    /***************** For Test ********************/
    public static void printTree(Node head) {
        System.out.println("Binary Tree:");
        printInOrder(head, 0, "H", 17);
        System.out.println();
    }

    public static void printInOrder(Node head, int height, String to, int len) {
        if (head == null) {
            return;
        }
        printInOrder(head.right, height + 1, "v", len);
        String val = to + head.value + to;
        int lenM = val.length();
        int lenL = (len - lenM) / 2;
        int lenR = len - lenM - lenL;
        val = getSpace(lenL) + val + getSpace(lenR);
        System.out.println(getSpace(height * len) + val);
        printInOrder(head.left, height + 1, "^", len);
    }

    public static String getSpace(int num) {
        String space = " ";
        StringBuffer buf = new StringBuffer("");
        for (int i = 0; i < num; i++) {
            buf.append(space);
        }
        return buf.toString();
    }

    public static boolean isBST(Node head) {
        if (head == null) {
            return true;
        }
        Node cur = head;
        Node mostRight = null;
        Integer pre = null;
        boolean ans = true;
        while (cur != null) {
            mostRight = cur.left;
            if (mostRight != null) {
                while (mostRight.right != null && mostRight.right != cur) {
                    mostRight = mostRight.right;
                }
                if (mostRight.right == null) {
                    mostRight.right = cur;
                    cur = cur.left;
                    continue;
                } else {
                    mostRight.right = null;
                }
            }
            if (pre != null && pre >= cur.value) {
                ans = false;
            }
            pre = cur.value;
            cur = cur.right;
        }
        return ans;
    }

    public static void main(String[] args) {
        Node head = new Node(4);
        head.left = new Node(2);
        head.right = new Node(6);
        head.left.left = new Node(1);
        head.left.right = new Node(3);
        head.right.left = new Node(5);
        head.right.right = new Node(7);
        printTree(head);
        morrisPre(head);
        morrisIn(head);
        morrisPost(head);
    }
}

package algorithm.training.struct.list;

public class D03_02_DeleteGivenValue {
    public static class Node {
        public int value;
        public Node next;

        public Node(int data) {
            this.value = data;
        }
    }

    // 自己的实现
    public static Node removeValue(Node head, int num) {
        if (head == null) return null;

        Node ans = head.value != num ? head : null;
        Node cur = head, pre = head;
        while (cur != null) {
            if (cur.value != num) {
                if (ans == null) ans = cur;
            } else {
                while (cur.next != null && cur.next.value == num) {
                    cur = cur.next;
                }
                pre.next = cur.next;
            }
            pre = cur;
            cur = cur.next;
        }
        return ans;
    }

    // 别人的实现，相对来说逻辑更加清晰
    public static Node removeValue2(Node head, int num) {
        // head来到第一个不需要删的位置
        while (head != null) {
            if (head.value != num) break;
            head = head.next;
        }

        Node pre = head, cur = head;
        while (cur != null) {
            if (cur.value == num) {
                pre.next = cur.next;
            } else {
                pre = cur;
            }
            cur = cur.next;
        }
        return head;
    }
    /************************* For Test **********************/
    public static void print(Node head) {
        while (head != null) {
            System.out.print(head.value + " ");
            head = head.next;
        }
        System.out.println();
    }

    public static void main(String[] args) {
        Node head = new Node(1);
        Node node1 = new Node(1);
        head.next = node1;
        Node node2 = new Node(2);
        node1.next = node2;
        Node node3 = new Node(3);
        node2.next = node3;
        Node node4 = new Node(3);
        node3.next = node4;
        Node node5 = new Node(3);
        node4.next = node5;
        Node node6 = new Node(4);
        node5.next = node6;
        Node node7 = new Node(5);
        node6.next = node7;

        // Node ans = removeValue(head, 1);
        // Node ans = removeValue(head, 3);
        // Node ans = removeValue(head, 5);
        Node ans = removeValue(head, 6);
        print(ans);
    }
}

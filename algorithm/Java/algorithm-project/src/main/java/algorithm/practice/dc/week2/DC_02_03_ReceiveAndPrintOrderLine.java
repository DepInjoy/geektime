package algorithm.practice.dc.week2;

import javax.xml.stream.events.NotationDeclaration;
import java.util.HashMap;

/**
 * @brief:      已知一个消息流会不断地吐出整数 1~N，但不一定按照顺序依次吐出
 *              如果上次打印的序号为i， 那么当i+1出现时
 *              请打印 i+1 及其之后接收过的并且连续的所有数
 *              直到1~N全部接收并打印完
 *              请设计这种接收并打印的结构
 * */
public class DC_02_03_ReceiveAndPrintOrderLine {
    public static class Node {
        public String info;
        public Node next;

        public Node(String info) {
            this.info = info;
        }
    }

    public static class MessageBox {
        private int waitingIndex;
        private HashMap<Integer, Node>    container;

        public MessageBox() {
            waitingIndex = 1;
            container = new HashMap<>();
        }

        public void receive(int num, String info) {
            if (num < 1) return;

            Node curNode = new Node(info);
            if (container.containsKey(num + 1)) {
                curNode.next = container.get(num + 1);
                container.put(num, curNode);
            }

            if (container.containsKey(num - 1)) {
                container.get(num - 1).next = curNode;
            }
            container.put(num, curNode);

            // System.out.println("trigger " + waitingIndex);
            if (container.containsKey(waitingIndex)) {
                print(container.get(waitingIndex));
            }
        }


        private void print(Node head) {
            Node cur = head;
            while (cur != null) {
                System.out.print(cur.info + " ");
                container.remove(waitingIndex);
                waitingIndex++;
                cur = cur.next;
            }
            // System.out.println();
        }
    }

    public static void main(String[] args) {
        // MessageBox only receive 1~N
        MessageBox box = new MessageBox();
        // 1....
        System.out.println("这是2来到的时候");
        box.receive(2,"B"); // - 2"
        System.out.println("这是1来到的时候");
        box.receive(1,"A"); // 1 2 -> print, trigger is 1
        box.receive(4,"D"); // - 4
        box.receive(5,"E"); // - 4 5
        box.receive(7,"G"); // - 4 5 - 7
        box.receive(8,"H"); // - 4 5 - 7 8
        box.receive(6,"F"); // - 4 5 6 7 8
        box.receive(3,"C"); // 3 4 5 6 7 8 -> print, trigger is 3
        box.receive(9,"I"); // 9 -> print, trigger is 9
        box.receive(10,"J"); // 10 -> print, trigger is 10
        box.receive(12,"L"); // - 12
        box.receive(13,"M"); // - 12 13
        box.receive(11,"K"); // 11 12 13 -> print, trigger is 11
    }
}

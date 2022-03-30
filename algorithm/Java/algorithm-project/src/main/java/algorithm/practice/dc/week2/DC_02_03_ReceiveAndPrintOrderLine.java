package algorithm.practice.dc.week2;

/**
 * @brief:      已知一个消息流会不断地吐出整数 1~N，但不一定按照顺序依次吐出
 *              如果上次打印的序号为i， 那么当i+1出现时
 *              请打印 i+1 及其之后接收过的并且连续的所有数
 *              直到1~N全部接收并打印完
 *              请设计这种接收并打印的结构
 * */
public class DC_02_03_ReceiveAndPrintOrderLine {
    public static class MessageBox {
        public void receive(int num, String info) {

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

package algorithm.practice.dc.week2;
/**
 * @brief:  贩卖机只支持硬币支付，且收退都只支持10 ，50，100三种面额
 *          一次购买只能出一瓶可乐，且投钱和找零都遵循优先使用大钱的原则
 *          需要购买的可乐数量是m，其中手头拥有的10、50、100的数量分别为a、b、c
 *          可乐的价格是x(x是10的倍数)
 *          请计算出需要投入硬币次数？
 * */
public class DC_02_02_Cola {
    public static int right(int m, int a, int b, int c, int x) {
        return 0;
    }

    public static int putTimes(int m, int a, int b, int c, int x) {
        return 0;
    }

    public static void main(String[] args) {
        int testTime = 1000;
        int zhangMax = 10;
        int colaMax = 10;
        int priceMax = 20;
        System.out.println("test begin");
        for (int i = 0; i < testTime; i++) {
            int m = (int) (Math.random() * colaMax);
            int a = (int) (Math.random() * zhangMax);
            int b = (int) (Math.random() * zhangMax);
            int c = (int) (Math.random() * zhangMax);
            int x = ((int) (Math.random() * priceMax) + 1) * 10;
            int ans1 = putTimes(m, a, b, c, x);
            int ans2 = right(m, a, b, c, x);
            if (ans1 != ans2) {
                System.out.println("int m = " + m + ";");
                System.out.println("int a = " + a + ";");
                System.out.println("int b = " + b + ";");
                System.out.println("int c = " + c + ";");
                System.out.println("int x = " + x + ";");
                break;
            }
        }
        System.out.println("test end");
    }
}

package algorithm.practice.dc.week2;

import javax.swing.plaf.IconUIResource;

/**
 * @brief:  贩卖机只支持硬币支付，且收退都只支持10 ，50，100三种面额
 *          一次购买只能出一瓶可乐，且投钱和找零都遵循优先使用大钱的原则
 *          需要购买的可乐数量是m，其中手头拥有的10、50、100的数量分别为a、b、c
 *          可乐的价格是x(x是10的倍数)
 *          请计算出需要投入硬币次数？
 *
 *          实现思想：
 *              1. 暴力尝试,每次只买一瓶可乐并找零
 *              2. 从最大面值开始，首先凑够一瓶可乐，将之前剩余较小的面值凑第一瓶可乐
 *                 之后计算当前面值剩余部分可以买几瓶可乐，将用不完的部分更新剩余的金额和张数
 *                 进行下一轮的计算
 * */
public class DC_02_02_Cola {
    // 暴力尝试,每次只买一瓶,并找零
    public static int right(int m, int a, int b, int c, int x) {
        int totalTimes = 0;
        int[] qians = {100, 50, 10};
        int[] zhangs = {c, b, a};
        while (m != 0) {
            int putTimes = buyOne(qians, zhangs, x);
            if (putTimes == -1) {
                return -1;
            }
            totalTimes += putTimes;
            m--;
        }
        return totalTimes;
    }


    public static int putTimes(int m, int a, int b, int c, int x) {
        int[] qians = { 100, 50, 10 };
        int[] zhangs = {c, b, a};


        int times = 0;
        int preQianRest = 0, preZhangRest = 0;
        for (int i = 0; i < 3 && m != 0; i++) {
            // 当前面值买第一瓶可乐，需要多少张
            int curQianFirstBuyNeedZhang = (x - preQianRest + qians[i] - 1) / qians[i];
            if (zhangs[i] >= curQianFirstBuyNeedZhang) {
                zhangs[i] -= curQianFirstBuyNeedZhang;
                giveRest(qians, zhangs, i + 1,
                        preQianRest + qians[i] * curQianFirstBuyNeedZhang - x, 1);
                times += curQianFirstBuyNeedZhang + preZhangRest;
                m -= 1;
            } else {
                preQianRest += qians[i] * zhangs[i];
                preZhangRest += zhangs[i];
                continue;
            }

            // 当前面值的钱，买一瓶可乐需要几张
            int curQianBuyOneNeedZhang = (x + qians[i] - 1) / qians[i];
            // 用当前面值的钱，一共买几瓶可乐
            int curQianBuyCols = Math.min(zhangs[i] / curQianBuyOneNeedZhang, m);
            giveRest(qians, zhangs, i + 1, qians[i] * curQianBuyOneNeedZhang - x, curQianBuyCols);

            // 投了几次币
            times += curQianBuyCols * curQianBuyOneNeedZhang;
            // 还剩下多少瓶可乐
            m -= curQianBuyCols;
            // 当前面值剩下张数
            zhangs[i] -= curQianBuyCols * curQianBuyOneNeedZhang;
            preQianRest = zhangs[i] * qians[i];
            preZhangRest = zhangs[i];
        }
        return m == 0 ? times : -1;
    }

    // 返回买一瓶投币次数
    public static int buyOne(int[] qians, int[] zhangs, int reset) {
        int trySeq = -1;
        for (int i = 0; i < 3; i++) {
            if (zhangs[i] != 0) {
                trySeq = i;
                break;
            }
        }

        // 没钱了
        if (trySeq == -1) return -1;

        if (qians[trySeq] >= reset) {
            zhangs[trySeq]--;
            giveRest(qians, zhangs, trySeq + 1, qians[trySeq] - reset, 1);
            return 1;
        } else {
            zhangs[trySeq]--;
            int putTimes = buyOne(qians, zhangs, reset - qians[trySeq]);
            if (putTimes == -1) {
                return -1;
            }
            return putTimes + 1;
        }

    }

    // 找零
    public static void giveRest(int[] qians, int[] zhangs, int index, int oneTimeRest, int times) {
        for (; index < 3; index++) {
            zhangs[index] += (oneTimeRest / qians[index]) * times;
            oneTimeRest = oneTimeRest % qians[index];
        }
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
            int ans1 = right(m, a, b, c, x);
            int ans2 = putTimes(m, a, b, c, x);
            if (ans1 != ans2) {
                System.out.println("m = " + m + ";");
                System.out.println("a = " + a + ";");
                System.out.println("b = " + b + ";");
                System.out.println("c = " + c + ";");
                System.out.println("x = " + x + ";");
                System.out.println("ans1= " + ans1 + " ans2= " + ans2);
                break;
            }
        }
        System.out.println("test end");
    }
}

package algorithm.practice.dc.week1;

import java.util.Arrays;

/**
 * @brief:      给定两个非负数组x和hp，长度都是N，再给定一个正数range
 *              x有序，x[i]表示i号怪兽在x轴上的位置；hp[i]表示i号怪兽的血量
 *              再给定一个正数range，表示如果法师释放技能的范围长度
 *              被打到的每只怪兽损失1点血量。
 *              返回要把所有怪兽血量清空，至少需要释放多少次AOE技能？
 *
 *              实现思想：
 *                  1. 暴力尝试
 *                      计算每个位置的覆盖范围，释放AOE将相应位置清零，计算最少次数
 *
 *                  2. 贪心
 *                      永远让最左边缘以最优的方式(AOE尽可能往右扩，最让最左边缘盖住目前怪的最左)变成0，也就是选择
 *                      一定能覆盖到最左边缘, 但是尽量靠右的中心点
 *                      等到最左边缘变成0之后，再去找下一个最左边缘...
 *
 *                 3. 基于贪心的思想用线段树实现，时间复杂度O(NlogN)
 * */
public class DC_01_06_AOE {
    // 暴力尝试
    public static int minAoe(int[] x, int[] hp, int range) {
        if (x == null || hp == null || x.length == 0 || hp.length == 0
                || x.length != hp.length || range <= 0) return -1;

        int N = x.length;
        // 如果在i位置释放AOE,受影响的范围
        int[] coverLeft = new int[N];
        int[] coverRight = new int[N];
        int left = 0, right = 0;
        for(int i = 0; i < N; i++) {
            while (x[i] - x[left] > range) {
                left++;
            }
            while (right < N && x[right] - x[i] <= range) {
                right++;
            }
            coverLeft[i] = left;
            coverRight[i] = right - 1;
        }
        return AOEprocess(hp, coverLeft, coverRight);
    }

    public static int minAoe21(int[] x, int[] hp, int range) {
        int N = x.length;
        int ans = 0;
        for (int i = 0; i < N; i++) {
            if (hp[i] > 0) {
                int triggerPost = i;
                while (triggerPost < N && x[triggerPost] - x[i] <= range) {
                    triggerPost++;
                }
                ans += hp[i];
                aoe(x, hp, i, triggerPost - 1, range);
            }
        }
        return ans;
    }

    public static void aoe(int[] x, int[] hp, int L, int trigger, int range) {
        int N = x.length;
        int RPost = trigger;
        while (RPost < N && x[RPost] - x[trigger] <= range) {
            RPost++;
        }
        int minus = hp[L];
        for (int i = L; i < RPost; i++) {
            hp[i] = Math.max(0, hp[i] - minus);
        }
    }

    // 贪心
    public static int minAoe1(int[] x, int[] hp, int range) {
        if (x == null || hp ==null || x.length == 0 || hp.length == 0
                || x.length != hp.length || range <= 0) return -1;

        int[] bak = new int[hp.length];
        for (int i = 0; i < hp.length; i++) {
            bak[i] = hp[i];
        }

        int ans = 0;
        int left = 0, pos = 1, right = 0;
        for(int i = 0; i < bak.length; i++) {
            if (bak[i] <= 0) {
                continue;
            } else {
                left = i;
                pos = left + 1;
            }
            while (pos < x.length && x[pos] - x[left] <= range) {
                pos++;
            }
            right = pos;
            while (right < x.length && x[right] - x[pos-1] <= range) {
                right++;
            }
            ans += bak[left];
            int tmp = bak[left];
            for (int j = left; j < right; j++) {
                bak[j] -= tmp;
            }
        }
        return ans;
    }

    // 贪心策略, 参考别人的实现，该实现更加清晰
    public static int minAoe11(int[] x, int[] hp, int range) {
        if (x == null || hp ==null || x.length == 0 || hp.length == 0
                || x.length != hp.length || range <= 0) return -1;

        int[] data = new int[hp.length];
        for (int i = 0; i < hp.length; i++) {
            data[i] = hp[i];
        }

        int ans = 0;
        for(int i = 0; i < hp.length; i++) {
            if (hp[i] > 0) {
                int triggerPos = i;
                while (triggerPos < hp.length && x[triggerPos] - x[i] <= range) {
                    triggerPos++;
                }
                ans += data[i];
                AOE(x, data, i, triggerPos-1, range);
            }
        }
        return ans;
    }

    public static void AOE(int[] x, int[] hp, int left, int pos, int range) {
        int right = pos;
        while (right < x.length && x[right] - x[pos] <= range) {
            right++;
        }
        int tmp = hp[left];
        for (int i = left; i < right; i++) {
            hp[i] = Math.max(0, hp[i] - tmp);
        }
    }

    public static int AOEprocess(int[] hp, int[] coverLeft, int[] coverRight) {
        int N = hp.length;
        int ans = Integer.MAX_VALUE;
        for (int i = 0; i < N; i++) {
            // 左侧的血量清零
            for (int j = coverLeft[i]; j <= coverRight[i]; j++) {
                if (hp[j] > 0) {
                    int[] next = AOE(hp, coverLeft[i], coverRight[i]);
                    ans = Math.min(ans, AOEprocess(next, coverLeft, coverRight) + 1);
                    break;
                }
            }
        }
        return ans == Integer.MAX_VALUE ? 0 : ans;
    }

    // 释放一次AOE,影响范围[L,R]
    public static int[] AOE(int[] hp, int L, int R) {
        int[] next = new int[hp.length];
        for (int i = 0; i < hp.length; i++) {
            if (i >= L && i <= R) {
                next[i] = hp[i] - 1;
            } else {
                next[i] = hp[i];
            }
        }
        return next;
    }

    public static int minAoe2(int[] x, int[] hp, int range) {
        if (x == null || hp ==null || x.length == 0 || hp.length == 0
                || x.length != hp.length || range <= 0) return -1;

        return 0;
    }

    /******************** For Test *******************/
    public static int[] randomArray(int n, int valueMax) {
        int[] ans = new int[n];
        for (int i = 0; i < n; i++) {
            ans[i] = (int) (Math.random() * valueMax) + 1;
        }
        return ans;
    }

    public static int[] copyArray(int[] arr) {
        int N = arr.length;
        int[] ans = new int[N];
        for (int i = 0; i < N; i++) {
            ans[i] = arr[i];
        }
        return ans;
    }

    public static void printInput(int[] x, int[] hp, int range) {
        System.out.print("x: ");
        for (int i = 0; i < x.length; i++) {
            System.out.print(x[i] + " ");
        }
        System.out.println();

        System.out.print("hp: ");
        for (int i = 0; i < hp.length; i++) {
            System.out.print(hp[i] + " ");
        }
        System.out.println();

        System.out.println("range: " + range);
    }

    public static void main(String[] args) {
        int[] x0 = {1, 1, 2, 3, 5};
        int[] hp0 = {3, 2, 1, 2, 5};
        int range0 = 2;
        System.out.println(minAoe1(x0, hp0, range0));
        System.out.println(minAoe(x0, hp0, range0));
        System.out.println(minAoe21(x0, hp0, range0));

        int NMax = 5;
        int X = 5;
        int Hmax = 5;
        int R = 5;
        int time = 1000;
        System.out.println("test begin");
        for (int i = 0; i < time; i++) {
            int len = (int) (Math.random() * NMax) + 1;
            int[] x = randomArray(len, X);
            Arrays.sort(x);
            int[] hp = randomArray(len, Hmax);
            int range = (int) (Math.random() * R) + 1;

            // minAoe暴力尝试很慢,测试需要把数据量(N, X, H, R, time)改小
            // int[] x2 = copyArray(x);
            // int[] hp2 = copyArray(hp);
            // int ans2 = minAoe(x2, hp2, range);
            int[] x3 = copyArray(x);
            int[] hp3 = copyArray(hp);
            int ans3 = minAoe1(x3, hp3, range);
            int[] x4 = copyArray(x);
            int[] hp4 = copyArray(hp);
            int ans4 = minAoe11(x4, hp4, range);
            // if (ans2 != ans3 || ans3 != ans4) {
            //    System.out.println("Oops!");
            //    printInput(x, hp, range);
            //    System.out.println(ans2 + "," + ans3 + "," + ans4);
            // }
        }
        System.out.println("test end");
    }
}

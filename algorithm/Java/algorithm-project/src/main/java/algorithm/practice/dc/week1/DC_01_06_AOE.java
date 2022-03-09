package algorithm.practice.dc.week1;

import java.util.Arrays;

/**
 * @brief:      给定两个非负数组x和hp，长度都是N，再给定一个正数range
 *              x有序，x[i]表示i号怪兽在x轴上的位置；hp[i]表示i号怪兽的血量
 *              再给定一个正数range，表示如果法师释放技能的范围长度
 *              被打到的每只怪兽损失1点血量。
 *              返回要把所有怪兽血量清空，至少需要释放多少次AOE技能？
 * */
public class DC_01_06_AOE {
    public static int minAoe1(int[] x, int[] hp, int range) {
        return 0;
    }

    public static int minAoe2(int[] x, int[] hp, int range) {
        return 0;
    }

    public static int minAoe3(int[] x, int[] hp, int range) {
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

    public static void main(String[] args) {
        int N = 500;
        int X = 10000;
        int H = 50;
        int R = 10;
        int time = 5000;
        System.out.println("test begin");
        for (int i = 0; i < time; i++) {
            int len = (int) (Math.random() * N) + 1;
            int[] x = randomArray(len, X);
            Arrays.sort(x);
            int[] hp = randomArray(len, H);
            int range = (int) (Math.random() * R) + 1;
            int[] x2 = copyArray(x);
            int[] hp2 = copyArray(hp);
            int ans2 = minAoe2(x2, hp2, range);
            // 已经测过下面注释掉的内容，注意minAoe1非常慢，
            // 所以想加入对比需要把数据量(N, X, H, R, time)改小
//			int[] x1 = copyArray(x);
//			int[] hp1 = copyArray(hp);
//			int ans1 = minAoe1(x1, hp1, range);
//			if (ans1 != ans2) {
//				System.out.println("Oops!");
//				System.out.println(ans1 + "," + ans2);
//			}
            int[] x3 = copyArray(x);
            int[] hp3 = copyArray(hp);
            int ans3 = minAoe3(x3, hp3, range);
            if (ans2 != ans3) {
                System.out.println("Oops!");
                System.out.println(ans2 + "," + ans3);
            }
        }
        System.out.println("test end");
    }
}

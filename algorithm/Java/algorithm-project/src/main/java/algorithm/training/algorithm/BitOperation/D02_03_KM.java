package algorithm.training.algorithm.BitOperation;

import java.util.HashSet;

/**
 * @brief:      一个数组中有一种数出现K次，其他数都出现了M次，M > 1,  K < M
 *              找到，出现了K次的数，
 *              要求，额外空间复杂度O(1)，时间复杂度O(N)
 * */
public class D02_03_KM {
    public static int onlyKTimes(int[] arr, int k, int m) {
        return 0;
    }

    public static int test(int[] arr, int k, int m) {
        return 0;
    }

    public static int[] randomArray(int maxKinds, int range, int k, int m) {
        int ktimeNum = randomNumber(range);
        int times = Math.random() < 0.5 ? k : ((int) (Math.random() * (m - 1)) + 1);
        int numKinds = (int) (Math.random() * maxKinds) + 2;
        int[] arr = new int[times + (numKinds - 1) * m]; // k * 1 + (numKinds - 1) * m
        int index = 0;
        for (; index < times; index++) {
            arr[index] = ktimeNum;
        }
        numKinds--;
        HashSet<Integer> set = new HashSet<>();
        set.add(ktimeNum);
        while (numKinds != 0) {
            int curNum = 0;
            do {
                curNum = randomNumber(range);
            } while (set.contains(curNum));
            set.add(curNum);
            numKinds--;
            for (int i = 0; i < m; i++) {
                arr[index++] = curNum;
            }
        }
        // arr 填好了
        for (int i = 0; i < arr.length; i++) {
            // i 位置的数，我想随机和j位置的数做交换
            int j = (int) (Math.random() * arr.length);// 0 ~ N-1
            int tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
        }
        return arr;
    }

    // [-range, +range]
    public static int randomNumber(int range) {
        return ((int) (Math.random() * range) + 1) - ((int) (Math.random() * range) + 1);
    }

    public static void main(String[] args) {
        int kinds = 5;
        int range = 30;
        int testTime = 100000;
        int max = 9;
        System.out.println("测试开始");
        for (int i = 0; i < testTime; i++) {
            int a = (int) (Math.random() * max) + 1; // a 1 ~ 9
            int b = (int) (Math.random() * max) + 1; // b 1 ~ 9
            int k = Math.min(a, b);
            int m = Math.max(a, b);
            if (k == m) {  // k < m
                m++;
            }
            int[] arr = randomArray(kinds, range, k, m);
            int ans1 = test(arr, k, m);
            int ans2 = onlyKTimes(arr, k, m);
            if (ans1 != ans2) {
                System.out.println(ans1);
                System.out.println(ans2);
                System.out.println("出错了！");
            }
        }
        System.out.println("测试结束");
    }
}

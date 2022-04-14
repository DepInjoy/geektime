package algorithm.practice.dc.week3;

import jdk.internal.org.objectweb.asm.tree.analysis.Value;

import java.util.Arrays;
import java.util.HashSet;

/**
 * @brief:      给定一个数组arr，代表每个人的能力值。再给定一个非负数k
 *              如果两个人能力差值正好为k，那么可以凑在一起比赛
 *              一局比赛只有两个人，返回最多可以同时有多少场比赛
 *
 *              实现思想：
 *                  1. 排序+窗口，记录窗口右边使用的信息，避免重复使用
 * */
public class L03_04_MaxPairNumber {
    public static int maxPairNum1(int[] arr, int k) {
        if (arr == null || arr.length < 2 || k < 0) return -1;

        return process(arr, 0, k);
    }

    public static int process(int[] arr, int index, int k) {
        int ans = 0;
        if (index == arr.length) {
            for (int i = 1; i < arr.length; i += 2) {
                if (arr[i] - arr[i - 1] == k) {
                    ans++;
                }
            }
        } else {
            for (int r = index; r < arr.length; r++) {
                swap(arr, index, r);
                ans = Math.max(ans, process(arr, index + 1, k));
                swap(arr, index, r);
            }
        }
        return ans;
    }

    public static void swap(int[] arr, int i, int j) {
        int tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }

    public static int maxPairNum2(int[] arr, int k) {
        if (arr == null || arr.length < 2 || k < 0) return -1;

        int N = arr.length;
        int games = 0;
        Arrays.sort(arr);
        int left = 0, right = 1;
        boolean[] usedR = new boolean[N];
        while (left <= right && right < arr.length) {
            if (usedR[left]) {
                left++;
            } else if (left == right) {
                right++;
            } else {
                if (arr[right] - arr[left] < k) {
                    right++;
                } else if (arr[right] - arr[left] == k) {
                    games++;
                    usedR[right++] = true;
                    left++;
                } else {
                    left++;
                }
            }
        }
        return games;
    }

    /**************************** For Test ********************/
    public static int[] randomArray(int len, int value) {
        int[] arr = new int[len];
        for (int i = 0; i < arr.length; i++) {
            arr[i] = (int) (Math.random() * value);
        }
        return arr;
    }

    public static void printArray(int[] arr) {
        for (int i = 0; i < arr.length; i++) {
            System.out.print(arr[i] + " ");
        }
        System.out.println();
    }

    public static int[] copyArray(int[] arr) {
        int[] ans = new int[arr.length];
        for (int i = 0; i < arr.length; i++) {
            ans[i] = arr[i];
        }
        return ans;
    }

    public static void main(String[] args) {
        int maxLen = 10;
        int maxValue = 20;
        int maxK = 5;
        int testTime = 10000;
        System.out.println("功能测试开始");
        for (int i = 0; i < testTime; i++) {
            int N = (int) (Math.random() * (maxLen + 1));
            int[] arr = randomArray(N, maxValue);
            int[] arr1 = copyArray(arr);
            int[] arr2 = copyArray(arr);
            int k = (int) (Math.random() * (maxK + 1));
            int ans1 = maxPairNum1(arr1, k);
            int ans2 = maxPairNum2(arr2, k);
            if (ans1 != ans2) {
                System.out.println("Oops!");
                printArray(arr);
                System.out.println("k= " + k);
                System.out.println(ans1);
                System.out.println(ans2);
                break;
            }
        }
        System.out.println("功能测试结束");
    }
}

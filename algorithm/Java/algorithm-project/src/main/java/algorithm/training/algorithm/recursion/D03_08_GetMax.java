package algorithm.training.algorithm.recursion;

import java.util.TreeMap;

/**
 * @brief:      用递归方法求数组arr[L..R]中的最大值
 *
 *              实现思路:
 *                  1. 将[L..R]范围分成左右两半。左：[L..Mid]  右[Mid+1..R]
 *                  2. 左部分求最大值，右部分求最大值(是个递归过程，当范围上只剩下一个数便结束递归)
 *                  3. [L..R]范围上的最大值，是max{左部分最大值，右部分最大值}
 * */
public class D03_08_GetMax {
    public static int getMax(int[] arr) {
        if (arr == null || arr.length == 0) return Integer.MIN_VALUE;

        return getMax(arr, 0, arr.length - 1);
    }

    public static int getMax(int[] arr, int left, int right) {
        if (left == right) return arr[left];

        int mid = (left + right) >> 1;
        return Math.max(getMax(arr, left, mid), getMax(arr, mid + 1, right));
    }

    public static void main(String[] args) {
        int[] arr = {3, 6, 9, 32, 76, 9, 5};
        System.out.println(getMax(arr));
    }
}

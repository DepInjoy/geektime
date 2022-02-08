package algorithm.training.algorithm.BinarySearch;

import java.util.Arrays;

/**
 * @brief:      在一个有序数组中，找<=某个数最右侧的位置
 * */
public class C01_05_BSNearRight {
    public static int nearestIndex(int[] arr, int value) {
        if (arr == null || arr.length == 0) return -1;

        int L = 0, R = arr.length - 1;
        int index = -1;
        while (L <= R) {
            int mid = L + ((R - L) >> 1);
            if (arr[mid] <= value) {
                index = mid;
                L = mid + 1;
            } else {
                R = mid - 1;
            }
        }
        return index;
    }

    /********************** For Test ***************************/
    public static int test(int[] arr, int value) {
        if (arr == null) return -1;

        for (int i = arr.length - 1; i >= 0; i--) {
            if (arr[i] <= value) {
                return i;
            }
        }
        return -1;
    }

    // for test
    public static int[] generateRandomArray(int maxSize, int maxValue) {
        int[] arr = new int[(int) ((maxSize + 1) * Math.random())];
        for (int i = 0; i < arr.length; i++) {
            arr[i] = (int) ((maxValue + 1) * Math.random()) - (int) (maxValue * Math.random());
        }
        return arr;
    }

    public static void main(String[] args) {
        int testTime = 500000;
        int maxSize = 10;
        int maxValue = 100;
        boolean succeed = true;
        // 容错测试
        if (test(null, 0) != nearestIndex(null, 0)) {
            System.out.println("Oh NO!");
            return;
        }
        for (int i = 0; i < testTime; i++) {
            int[] arr = generateRandomArray(maxSize, maxValue);
            Arrays.sort(arr);
            int value = (int) ((maxValue + 1) * Math.random()) - (int) (maxValue * Math.random());
            if (test(arr, value) != nearestIndex(arr, value)) {
                succeed = false;
                break;
            }
        }
        System.out.println(succeed ? "Nice!" : "Oh NO!");
    }
}
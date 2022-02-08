package algorithm.training.algorithm.BinarySearch;

import java.util.Arrays;

/**
 * @brief:      在一个有序数组中，找某个数是否存在
 *
 *              时间复杂度为O(logN)
 * */
public class C01_04_BSExist {
    public static boolean exist(int[] sortedArr, int num) {
        if (sortedArr == null || sortedArr.length == 0) return false;

        int L = 0, R = sortedArr.length - 1;
        while (L <= R) {
            int mid = L + ((R - L) >> 1);
            if (sortedArr[mid] < num) {
                L = mid + 1;
            } else if (sortedArr[mid] > num) {
                R = mid - 1;
            } else {
                return true;
            }
        }
        return false;
    }

    /******************* For Test ***************************/
    // 生成一批最大值为maxValue为随机数字
    public static int[] generateRandomArray(int maxSize, int maxValue) {
        int[] arr = new int[(int) ((maxSize + 1) * Math.random())];
        for (int i = 0; i < arr.length; i++) {
            arr[i] = (int) ((maxValue + 1) * Math.random()) - (int) (maxValue * Math.random());
        }
        return arr;
    }

    // 暴力查找
    public static boolean test(int[] sortedArr, int num) {
        for(int cur : sortedArr) {
            if(cur == num) return true;
        }
        return false;
    }

    public static void main(String[] args) {
        int testTime = 500000;
        int maxSize = 10;
        int maxValue = 100;
        boolean succeed = true;
        for (int i = 0; i < testTime; i++) {
            int[] arr = generateRandomArray(maxSize, maxValue);
            Arrays.sort(arr);
            int value = (int) ((maxValue + 1) * Math.random()) - (int) (maxValue * Math.random());
            if (test(arr, value) != exist(arr, value)) {
                succeed = false;
                break;
            }
        }
        System.out.println(succeed ? "Nice!" : "Fucking fucked!");
    }
}

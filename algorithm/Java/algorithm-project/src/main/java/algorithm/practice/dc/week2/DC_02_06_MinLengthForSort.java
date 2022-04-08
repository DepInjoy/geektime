package algorithm.practice.dc.week2;

import java.util.Arrays;

/**
 * @brief:      给定一个数组arr，只能对arr中的一个子数组排序，
 *              但是想让arr整体都有序, 返回满足这一设定的子数组中，最短是多长
 *
 *              实现思想：
 *                  step1. 假设存在Ri将数组分成左右两部分，使得右边部分所有元素
 *                         大于左边最大值且右边部分单调递增
 *                  step2. 假设存在Li将数组分成左右两部分，使得左边部分所有元素
 *                          小于右边最小值且左边部分单调递减
 *                  则对[Li, Ri]排序，arr整体有序
 * */
public class DC_02_06_MinLengthForSort {
    public static int getMinLength(int[] arr) {
        if (arr == null || arr.length <= 1) return 0;

        int N = arr.length;
        int max = arr[0];
        int mostRightIndex = -1;
        for (int i = 1; i < N; i++) {
            if (arr[i] < max) {
                mostRightIndex = i;
            } else {
                max = arr[i];
            }
        }

        if (mostRightIndex == -1) return 0;

        int min = arr[N-1];
        int mostLeftIndex = 0;
        for (int i = N - 2; i >= 0; i--) {
            if (arr[i] > min) {
                mostLeftIndex = i;
            } else {
                min = arr[i];
            }
        }
        return mostRightIndex - mostLeftIndex + 1;
    }

    public static int getMinLength2(int[] arr) {
        if (arr == null || arr.length <= 1) return 0;

        int N = arr.length;
        int[] arr2 = new int[N];
        for (int i = 0; i < N; i++) {
            arr2[i] = arr[i];
        }
        Arrays.sort(arr2);

        int ans = N;
        for (int left = 0; left < N; left++) {
            for (int right = N; right >= left; right--) {
                if (process(arr, left, right, arr2)) {
                    ans = Math.min(right - left, ans);
                }
            }
        }
        return ans;
    }

    public static boolean process(int[] arr, int left, int right, int[] arr2) {
        int N = arr.length;
        int[] carr = new int[N];
        for (int i = 0; i < N; i++) {
            carr[i] = arr[i];
        }
        Arrays.sort(carr, left, right);

        for (int i = 0; i < N; i++) {
            if (carr[i] != arr2[i]) return false;
        }
        return true;
    }

    /********************* For Test ****************************/
    public static int[] randomArray(int len, int value) {
        int[] ans = new int[len];
        for (int i = 0; i < ans.length; i++) {
            ans[i]= (int) (Math.random() * value);
        }
        return ans;
    }

    public static void printArray(int[] array) {
        for (int i = 0; i < array.length; i++) {
            System.out.print(array[i] + " ");
        }
        System.out.println();
    }

    public static void main(String[] args) {
        // 1, 2, 4, 7, 10, 11, 7, 12, 6, 7, 16, 18, 19
        //          *                    *
        int[] arr = { 1, 2, 4, 7, 10, 11, 7, 12, 6, 7, 16, 18, 19 };
        System.out.println(getMinLength(arr)); // 7
        System.out.println(getMinLength2(arr)); // 7

        int[] arr2 = { 2, 1 };
        System.out.println(getMinLength(arr2)); // 2
        System.out.println(getMinLength2(arr2)); // 2

        int[] arr3 = { 17, 59, 32, 56, 55, 37, 96, 72};
        System.out.println(getMinLength(arr3)); // 7
        System.out.println(getMinLength2(arr3)); // 7

        // 对数器
        int N = 10;
        int value = 100;
        int testTime = 50000;
        System.out.println("测试开始");
        for (int i = 0; i < testTime; i++) {
            int len = (int) (Math.random() * N) + 1;
            int[] array = randomArray(len, value);
            int ans1 = getMinLength(array);
            int ans2 = getMinLength2(array);
            if (ans1 != ans2) {
                printArray(array);
                System.out.println(ans1);
                System.out.println(ans2);
                System.out.println("Oops!");
                break;
            }
        }
        System.out.println("测试结束");
    }
}

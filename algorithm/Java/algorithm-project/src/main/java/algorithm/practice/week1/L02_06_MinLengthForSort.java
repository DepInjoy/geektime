package algorithm.practice.week1;

/**
 * @brief       给定一个数组arr，只能对arr中的一个子数组排序，
 *              但是想让arr整体都有序, 返回满足这一设定的子数组中，最短的是多长
 *
 */

public class L02_06_MinLengthForSort {
    public static int getMinLength(int[] arr) {
        if (arr == null || arr.length < 2) return 0;

        int left = 0, right = arr.length - 1;
        while (left+1 < arr.length && arr[left] <= arr[left+1]) {
            left++;
        }
        while (left <= right && arr[right-1] <= arr[right]) {
            right--;
        }
        return right - left + 1;
    }

    public static void main(String[] args) {
        int[] arr = { 1, 2, 4, 7, 10, 11, 7, 12, 6, 7, 16, 18, 19 };
        System.out.println(getMinLength(arr));

        int[] arr2 = { 1, 2, 4, 7, 10, 11 };
        System.out.println(getMinLength(arr2));
    }
}

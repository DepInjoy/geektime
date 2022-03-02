package algorithm.training.algorithm.sort;

/**
 * @brief:              选择排序
 *
 *              时间复杂度为O(N^2)
 * */
public class C01_01_SelectionSort {
    public static void selectionSort(int[] arr) {
        if (arr == null || arr.length < 2) return;

    }

    public static void swap(int[] arr, int i, int j) {
        int tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }

    public static void main(String[] args) {
        int[] arr0 = {1, 5, 3, 7, 2, 8};
        selectionSort(arr0);
        for (int item : arr0) {
            System.out.print(item + " ");
        }
    }
}

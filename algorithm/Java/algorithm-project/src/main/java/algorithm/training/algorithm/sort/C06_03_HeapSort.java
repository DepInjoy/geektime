package algorithm.training.algorithm.sort;

/**
 *  @brief:                     堆排序
 *              空间复杂度为O(1)，时间复杂度为O(NlogN)
* */
public class C06_03_HeapSort {
    public static void swap(int[] arr, int i1, int i2) {
        int tmp = arr[i1];
        arr[i1] = arr[i2];
        arr[i2] = tmp;
    }

    public static void heapify(int[] arr, int index, int heapSize) {
        int left = index * 2 + 1;
        while (left < heapSize) {
            int best = (left + 1 < heapSize && arr[left + 1] > arr[left]) ?
                    left + 1 : left;
            if (arr[best] > arr[index]) {
                swap(arr, best, index);
                left = 2 * left + 1;
            } else {
                break;
            }
        }
    }

    public static void HeapSort(int[] arr) {
        if (arr == null || arr.length <= 1) return;

        // 1. 建立大根堆
        for (int i = arr.length - 1; i >= 0; --i) {
            heapify(arr, i, arr.length);
        }

        // 2. 将堆顶元素和堆尾元素交换, 减少堆大小,直至堆大小缩减为0
        int heapSize = arr.length;
        swap(arr, 0 , --heapSize);  // 大根堆堆顶是最大值,移到最后
        while (heapSize > 0) {
            heapify(arr, 0, heapSize); // 堆调整
            swap(arr, 0 , --heapSize);
        }
    }

    public static void print(int arr[]) {
        for (int i = 0; i < arr.length; ++i) {
            System.out.print(arr[i] + " ");
        }
        System.out.println("");
    }

    public static void main(String[] args) {
        int[] arr0 = new int[]{9, 8, 7, 6, 5, 4, 3, 2, 1};
        HeapSort(arr0);
        print(arr0);
    }
}

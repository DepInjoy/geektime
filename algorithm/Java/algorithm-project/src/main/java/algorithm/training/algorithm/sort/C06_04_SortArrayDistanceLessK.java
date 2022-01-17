package algorithm.training.algorithm.sort;

import java.util.PriorityQueue;

/**
 * @brief:      已知一个几乎有序的数组。几乎有序是指，如果把数组排好顺序的话，每个元素移动的距离一定不超过k
 *              并且k相对于数组长度来说是比较小的。选择一个合适的排序策略，对这个数组进行排序
 * */
public class C06_04_SortArrayDistanceLessK {
    /**
     * @brief:      利用小根堆可以达到O(NlogK)的时间复杂度, 实现思路：
     *                  1. 将前K个元素插入小根堆中
     *                  2. 假设返回结果为arr index=0,arr[index]为堆顶元素，
     *                  3. 将输入的K+1个值insert到小根堆,arr[1]堆顶元素
     *                  4. 重复执行第三步，直到输入数组的尾部
     *                  5. 继续取小根堆中元素并插入到arr中
     *
     * */
    public static void sortedArrDistanceLessK(int[] arr, int k) {
        if (k == 0) return;

        PriorityQueue<Integer> heap = new PriorityQueue<Integer>();  // 小根堆
        Integer hi = 0, index = 0;
        // 将前k个元素插入小根堆,最小元素位于堆顶,堆调整时间复杂度为O(logK)
        for (; hi < Math.min(arr.length - 1, k - 1); ++hi) {
            heap.add(arr[hi]);
        }

        // 取堆顶最小元素，依次放入数组中
        for (; hi < arr.length; ++hi) {
            arr[index++] = heap.poll();
            heap.add(arr[hi]);
        }

        while (!heap.isEmpty()) {
            arr[index++] = heap.poll();
        }
    }

    public static void printArr(int[] array) {
        System.out.println("The input array is ");
        for (int i = 0; i < array.length; i++) {
            System.out.print(" " + array[i]);
        }
    }

    public static void main(String[] args) {
        int[] arr = new int[]{0, 1, 2, 3, 5, 4, 6, 9, 8, 7};
        Integer k = 3;
        sortedArrDistanceLessK(arr, k);
        printArr(arr);
    }
}

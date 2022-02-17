package algorithm.training.algorithm.bfprt;

import java.util.Comparator;
import java.util.PriorityQueue;

/**
 * @brief:      BFPRT算法(又称为中位数的中位数算法)
 *
 *              在一组数中求前K大或前k小问题,简称TOP-K问题,目前解决TOP-K问题最有效的算法即是BFPRT算法
 * */
public class D29_01_FindMinKth {
    // 利用bfprt算法查找第K小,时间复杂度为O(N)
    public static int minKth3(int[] array, int k) {
        int[] arr = copyArray(array);
        return bfprt(arr, 0, arr.length-1, k-1);
    }

    // 利用大根堆，时间复杂度O(N*logK)
    public static int minKth1(int[] arr, int k) {
        PriorityQueue<Integer> maxHeap = new PriorityQueue<>(new MaxHeapComparator());
        for (int i = 0; i < k; i++){
            maxHeap.add(arr[i]);
        }

        for (int i = k; i < arr.length;i++) {
            if (arr[i] < maxHeap.peek()) {
                maxHeap.poll();
                maxHeap.add(arr[i]);
            }
        }
        return maxHeap.peek();
    }

    // 改写快排，时间复杂度O(N)
    public static int minKth2(int[] array, int k) {
        int[] arr = copyArray(array);
        return process2(arr, 0, arr.length - 1, k - 1);
    }

    public static int process2(int[] arr, int L, int R, int index) {
        if (L == R) return arr[index];

        int pivot = arr[L + (int)(Math.random()*(R-L+1))];
        int[] range = partition(arr, L, R, pivot);
        if (range[0] <= index && index >= range[1]) {
            return arr[index];
        } else if (range[0] > index) {
            return process2(arr, index, range[0]-1, index);
        } else {
            return process2(arr, range[1] + 1, R, index);
        }
    }

    public static class MaxHeapComparator implements Comparator<Integer> {
        @Override
        public int compare(Integer o1, Integer o2) {
            return o2-o1;
        }
    }

    public static int bfprt(int[] arr, int L, int R, int index) {
        if (L == R) {
            return arr[L];
        }

        /*
            1. 获取主元
                1.1. 将n个元素按顺序分为n/5个组, 每组5个元素
                1.2. 对于n/5个组中的每一组使用插入排序找到它们各自的中位数
                1.3. 对于 1.2 中找到的所有中位数，调用BFPRT算法求出它们的中位数,作为主元
         */
        int pivot = medianOfMedians(arr, L, R);
        // 2. 以选取的主元为分界点，把小于主元的放在左边，大于主元的放在右边
        int[] range = partition(arr, L, R, pivot);
        // 3. 判断主元的位置与 k 的大小，有选择的对左边或右边递归
        if (index >= range[0] && index <= range[1]) {
            return arr[index];
        } else if (index < range[0]) {
            return bfprt(arr, L, range[0]-1, index);
        } else {
            return bfprt(arr, L, range[1] + 1, index);
        }
    }

    // 每五个数为一组,寻找中位数,将这一系列中位数组成新的数组,返回其中位数
    public static int medianOfMedians(int[] arr, int L, int R) {
        int size = R - L + 1;
        int offset = size % 5 == 0 ? 0 : 1;
        // 5个一组中位数组成的数组
        int[] mArr = new int[size/5+offset];
        for (int team = 0; team < mArr.length;team++) {
            int teamFirst = L + team * 5;
            mArr[team] = getMedian(arr, teamFirst, Math.min(R, teamFirst+4));
        }
        return bfprt(mArr, 0, mArr.length-1, mArr.length/2);
    }

    // 获取中位数
    public static int getMedian(int[] arr, int L, int R) {
        insertionSort(arr, L, R);
        return arr[(L + R) / 2];
    }

    // 插入排序
    public static void insertionSort(int[] arr, int L, int R) {
        for(int i = L+1;i <= R;i++) {
            for (int j = i-1;j>=L && arr[j]>arr[j+1];j++) {
                swap(arr, j, j+1);
            }
        }
    }

    public static int[] partition(int[] arr, int L, int R, int pivot) {
        int less = L - 1;
        int more = R + 1;
        int cur = L;
        while (cur < more) {
            if (arr[cur] < pivot) {
                swap(arr, ++less, cur++);
            } else if (arr[cur] > pivot) {
                swap(arr, cur, --more);
            } else {
                cur++;
            }
        }
        return new int[]{less, more};
    }

    public static void swap(int[] arr, int i, int j) {
        int tmp = arr[i];
        arr[i] = arr[j];
        arr[j] = tmp;
    }

    public static int[] copyArray(int[] arr) {
        int[] ans = new int[arr.length];
        for (int i = 0; i != ans.length; i++) {
            ans[i] = arr[i];
        }
        return ans;
    }

    /************************* For Test *************************/
    public static int[] generateRandomArray(int maxSize, int maxValue) {
        int[] arr = new int[(int) (Math.random() * maxSize) + 1];
        for (int i = 0; i < arr.length; i++) {
            arr[i] = (int) (Math.random() * (maxValue + 1));
        }
        return arr;
    }

    public static void main(String[] args) {
        int testTime = 1000000;
        int maxSize = 100;
        int maxValue = 100;
        System.out.println("test begin");
        for (int i = 0; i < testTime; i++) {
            int[] arr = generateRandomArray(maxSize, maxValue);
            int k = (int) (Math.random() * arr.length) + 1;
            int ans1 = minKth1(arr, k);
            int ans2 = minKth2(arr, k);
            int ans3 = minKth3(arr, k);
            if (ans1 != ans2 || ans2 != ans3) {
                System.out.println("Oops!");
            }
        }
        System.out.println("test finish");
    }
}

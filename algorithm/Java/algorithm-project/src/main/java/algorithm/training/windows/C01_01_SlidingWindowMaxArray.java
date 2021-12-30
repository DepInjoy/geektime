package algorithm.training.windows;

import java.util.LinkedList;

/**
 * 题目描述:
 *          假设一个固定大小为W的窗口，依次划过arr，返回每一次滑出状况的最大值
 *          例如，arr = [4,3,5,4,3,3,6,7], W = 3, 返回：[5,5,5,4,6,7]
 * */
public class C01_01_SlidingWindowMaxArray {
    public static int[] getMaxWindow(int[] arr, int w) {
        if (arr == null || w < 1 || arr.length < w) return null;

        // 存放从小到大的窗口最大值的索引
        LinkedList<Integer> qmax = new LinkedList<Integer>();
        int [] res = new int[arr.length - w + 1];
        Integer index = 0;
        for (Integer right = 0; right < arr.length; right++) {
            // 当前值比双端队列的头元素大,意味着找到更大的值,将相关元素从双端队列中弹出
            while (!qmax.isEmpty() && arr[qmax.peekLast()] <= arr[right]) {
                qmax.pollLast();
            }
            qmax.addLast(right);
            // 从双端队列的头部将过期数据弹出
            if (qmax.peekFirst() <= right - w) {
                qmax.pollFirst();
            }
            if (right >= w - 1) {  // 封装结果
                res[index++] = arr[qmax.peekLast()];
            }
        }
        return res;
    }
}

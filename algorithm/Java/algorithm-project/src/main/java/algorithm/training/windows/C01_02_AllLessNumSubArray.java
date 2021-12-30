package algorithm.training.windows;

import java.util.LinkedList;

/**
 * 给定一个整型数组arr, 和一个整数num, 某个arr中的子数组sub，如果想达标，必须满足：
 * sub中最大值 – sub中最小值 <= num，返回arr中达标子数组的数量
 * */
public class C01_02_AllLessNumSubArray {
    public static int getNum(int[] arr, int num) {
        if (arr == null || arr.length == 1) return 0;

        int res = 0;
        LinkedList<Integer> qmin = new LinkedList<Integer>();
        LinkedList<Integer> qmax = new LinkedList<Integer>();
        int left = 0, right = 0;
        while (left < arr.length) {
            while (right < arr.length) {
                // 更新当前窗口的最大值
                while (!qmax.isEmpty() && arr[qmax.peekLast()] <= arr[right]) {
                    qmax.pollLast();
                }
                qmax.addLast(right);

                // 更新当前窗口的最小值
                while (!qmin.isEmpty() && arr[qmin.peekLast()] >= arr[right]) {
                    qmin.pollLast();
                }
                qmin.addLast(right);

                if (arr[qmax.getLast()] - arr[qmin.getLast()] > num) {
                    break;
                }
                right++;
            }
            res += right - left;
            // 清理过期数据
            if (qmax.peekFirst() == left) {
                qmax.pollFirst();
            }
            if (qmin.peekFirst() == left) {
                qmin.pollFirst();
            }
            left++;
        }
        return res;
    }

    public static void main(String[] args) {
        int[] arr = new int[6];
        arr[0] = 0;
        arr[1] = 1;
        arr[2] = 2;
        arr[3] = 3;
        arr[4] = 6;
        arr[5] = 8;
        int num = 5, targetOutput = 21;
        assert(getNum(arr, num) != targetOutput);
        System.out.println(getNum(arr, num));
    }
}

package algorithm.training.windows;

import java.util.Stack;

/**
 *  给定一个只包含正数的数组arr，arr中任何一个子数组sub,
 * 一定都可以算出(sub累加和 )* (sub中的最小值)是什么,
 * 那么所有子数组中，这个值最大是多少?
 */
public class C01_04_AllTimesMinToMax {
    /*
    * brief:    实现思想:寻找某个值附近的大于该值左右最近的索引
    *               计算该子串的(sub累加和 )* (该值),寻找最大值
    * */
    public static int max(int[] arr) {
        if (arr == null || arr.length == 0) return -1;

        // 数据预处理
        int[] sum = new int[arr.length];
        sum[0] = arr[0];
        for (int i = 1; i < arr.length; i++) {
            sum[i] = sum[i - 1] + arr[i];
        }

        int res = Integer.MIN_VALUE;
        Stack<Integer> stack = new Stack<Integer>();
        for (Integer i = 0; i < arr.length; i++) {
            while (!stack.isEmpty() && arr[stack.peek()] >= arr[i]) { // 发现比arr[i]小的
                // 栈中是从栈底到栈顶由大到小的, 栈顶即子数组sub中的最小值
                // 接着,在这个有序的数组中寻找符合条件的值
                Integer index = stack.pop();
                res = Math.max(res, (stack.isEmpty() ? sum[i - 1] : (sum[i - 1] - sum[stack.peek()])) * arr[index]);
            }
            stack.push(i);
        }

        while (!stack.isEmpty()) {
            Integer index = stack.pop();
            res = Math.max(res, (stack.isEmpty() ?  sum[arr.length - 1] :  (sum[arr.length - 1] - sum[stack.peek()]))
                    * arr[index]);
        }
        return res;
    }

    public static void ut1() {
        int[] arr = new int[7];
        arr[0] = 1;
        arr[1] = 5;
        arr[2] = 2;
        arr[3] = 3;
        arr[4] = 3;
        arr[5] = 4;
        arr[6] = 1;
        assert max(arr) == 34;
    }

    public static void ut2() {
        int[] arr = new int[8];
        arr[0] = 1;
        arr[1] = 6;
        arr[2] = 5;
        arr[3] = 2;
        arr[4] = 3;
        arr[5] = 3;
        arr[6] = 4;
        arr[7] = 1;
        assert max(arr) == 55;
    }

    public static void main(String[] args) {
        ut1();
        ut2();
    }
}

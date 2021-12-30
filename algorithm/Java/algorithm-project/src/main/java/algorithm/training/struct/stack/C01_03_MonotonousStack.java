package algorithm.training.struct.stack;

import java.util.*;

/**
 *                  ** 单调栈 **
 *  一种特别设计的栈结构，为了解决如下的问题：
 *  给定一个可能含有重复值的数组arr，i位置的数一定存在如下两个信息
 *      1. arr[i]的左侧离i最近并且小于(或者大于)arr[i]的数在哪？
 *      2. arr[i]的右侧离i最近并且小于(或者大于)arr[i]的数在哪？
 *  如果想得到arr中所有位置的两个信息，怎么能让得到信息的过程尽量快。
 * */

public class C01_03_MonotonousStack {
    /*
     *   @brief:     寻找数组arr中离索引i的值最近的小于arr[i]的索引
     *               实现思想:将数据由小到大进行入栈,当发现比栈顶元素大的数据时,出栈
     *                   相邻元素即左边距离最近的小于arr[i]的元素索引
     *                   当前访问的索引即右边距离最近的小于arr[i]的元素索引
     *
     *   @param:     arr输入的数据，无重复值
     *
     *   @return     res[i][0]左边距离arr[i]最近的小于arr[i]的索引,其中i为arr的索引
     *               res[i][1]右边距离arr[i]最近的小于arr[i]的索引,其中i为arr的索引
     * */
    public static int[][] getNearLessNoRepeat(int[] arr) {
        if (arr == null || arr.length == 0) return null;

        int[][] res = new int[arr.length][2];
        // 从小到大
        Stack<Integer> orderedStack = new Stack<Integer>();
        for (int i = 0; i < arr.length; i++) {
            while (!orderedStack.isEmpty() && arr[orderedStack.peek()] > arr[i]) {
                int index = orderedStack.pop();
                int nearLeftLessIndex = orderedStack.isEmpty() ? -1 : orderedStack.peek();
                res[index][0] = nearLeftLessIndex;
                res[index][1] = i;
            }
            orderedStack.push(i);
        }

        while (!orderedStack.isEmpty()) {
            int index = orderedStack.pop();
            int nearLeftLessIndex = orderedStack.isEmpty() ? -1 : orderedStack.peek();
            res[index][0] = nearLeftLessIndex;
            res[index][1] = -1;
        }
        return res;
    }

    /*
    *   @param:     arr输入数组,允许元素重复
    *
    * */
    public static int[][] getNearLess(int[] arr) {
        if (arr == null || arr.length == 0) return null;

        int[][] res = new int[arr.length][2];
        Stack<List<Integer>> stack = new Stack<List<Integer>>();
        for (Integer i = 0; i < arr.length; i++) {
            while (!stack.isEmpty() && arr[stack.peek().get(0)] > arr[i]) {
                List<Integer> listIndex = stack.pop();
                int nearLeftLessIndex = stack.isEmpty() ? -1 : stack.peek().get(stack.peek().size() - 1);
                for (Integer index : listIndex) {
                    res[index][0] = nearLeftLessIndex;
                    res[index][1] = i;
                }
            }

            // update stack
            if (!stack.isEmpty() && arr[stack.peek().get(0)] == arr[i]) { // 发现重复元素
                stack.peek().add(i);
            } else {
                ArrayList<Integer> list = new ArrayList<Integer>();
                list.add(i);
                stack.push(list);
            }
        }

        while (!stack.isEmpty()) {
            List<Integer> listIndex = stack.pop();
            int nearLeftLessIndex = stack.isEmpty() ? -1 : stack.peek().get(stack.peek().size() - 1);
            for (Integer index : listIndex) {
                res[index][0] = nearLeftLessIndex;
                res[index][1] = -1;
            }
        }
        return res;
    }

    /******************** UT Start ************************/
    public static void DecreasingNonrepeatInput() {
        int[] array = new int[5];
        array[0] = 5;
        array[1] = 4;
        array[2] = 3;
        array[3] = 2;
        array[4] = 1;
        int[][] res = getNearLessNoRepeat(array);
        // valid the result
        assert (res[0][0] == -1 && res[0][1] == 1);
        assert (res[1][0] == -1 && res[1][1] == 2);
        assert (res[2][0] == -1 && res[2][1] == 3);
        assert (res[3][0] == -1 && res[3][1] == 4);
        assert (res[4][0] == -1 && res[4][1] == -1);
        System.out.println("DecreasingInput, the Result: ");
        for (int i = 0; i < array.length; i++) {
            System.out.println("\t" + res[i][0] + " : " + res[i][1]);
        }
    }

    public static void IncreasingNonrepeatInput() {
        int[] array = new int[5];
        array[0] = 1;
        array[1] = 2;
        array[2] = 3;
        array[3] = 4;
        array[4] = 5;
        int[][] res = getNearLessNoRepeat(array);
        // valid the result
        assert (res[0][0] == -1 && res[0][1] == -1);
        assert (res[1][0] == 0 && res[1][1] == -1);
        assert (res[2][0] == 1 && res[2][1] == -1);
        assert (res[3][0] == 2 && res[3][1] == -1);
        assert (res[4][0] == 3 && res[4][1] == -1);
        System.out.println("IncreasingInput, the Result: ");
        for (int i = 0; i < array.length; i++) {
            System.out.println("\t" + res[i][0] + " : " + res[i][1]);
        }
    }

    public static void UnorderedNonrepeatInput() {
        int[] array = new int[5];
        array[0] = 5;
        array[1] = 7;
        array[2] = 3;
        array[3] = 4;
        array[4] = 6;
        int[][] res = getNearLessNoRepeat(array);
        // valid the result
        assert (res[0][0] == -1 && res[0][1] == 2);
        assert (res[1][0] == 0 && res[1][1] == 2);
        assert (res[2][0] == -1 && res[2][1] == -1);
        assert (res[3][0] == 2 && res[3][1] == -1);
        assert (res[4][0] == 3 && res[4][1] == -1);
        System.out.println("UnorderedInput, the Result: ");
        for (int i = 0; i < array.length; i++) {
            System.out.println("\t" + res[i][0] + " : " + res[i][1]);
        }
    }

    public static void RepeatedInput() {
        int[] array = new int[5];
        array[0] = 5;
        array[1] = 2;
        array[2] = 3;
        array[3] = 3;
        array[4] = 4;
        int[][] res = getNearLess(array);
        // valid the result
        assert (res[0][0] == -1 && res[0][1] == 1);
        assert (res[1][0] == -1 && res[1][1] == -1);
        assert (res[2][0] == 1 && res[2][1] == -1);
        assert (res[3][0] == 1 && res[3][1] == -1);
        assert (res[4][0] == 3 && res[4][1] == -1);
        System.out.println("RepeatedInput, the Result: ");
        for (int i = 0; i < array.length; i++) {
            System.out.println("\t" + res[i][0] + " : " + res[i][1]);
        }
    }

    public static void UnorderedAllowedRepeatedInput() {
        int[] array = new int[5];
        array[0] = 5;
        array[1] = 7;
        array[2] = 3;
        array[3] = 4;
        array[4] = 6;
        int[][] res = getNearLessNoRepeat(array);
        // valid the result
        assert (res[0][0] == -1 && res[0][1] == 2);
        assert (res[1][0] == 0 && res[1][1] == 2);
        assert (res[2][0] == -1 && res[2][1] == -1);
        assert (res[3][0] == 2 && res[3][1] == -1);
        assert (res[4][0] == 3 && res[4][1] == -1);
        System.out.println("UnorderedInput, the Result: ");
        for (int i = 0; i < array.length; i++) {
            System.out.println("\t" + res[i][0] + " : " + res[i][1]);
        }
    }

    /******************** UT End ************************/

    public static void main(String[] args) {
        // Nonrepeat input unit test
        DecreasingNonrepeatInput();
        IncreasingNonrepeatInput();
        UnorderedNonrepeatInput();
        // Allow repeated input unit test
        RepeatedInput();
        UnorderedAllowedRepeatedInput();
    }
}


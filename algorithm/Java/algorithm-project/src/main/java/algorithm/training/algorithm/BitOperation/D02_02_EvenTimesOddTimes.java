package algorithm.training.algorithm.BitOperation;

/**
 * @brief:      一个数组中有一种数出现了奇数次，其他数都出现了偶数次，找到并打印这种数
 * */
public class D02_02_EvenTimesOddTimes {
    // 只有一种数出现奇数次
    public static void printOddTimesNum1(int[] arr) {
        if (arr == null || arr.length == 0) return;

        int ans = 0;
        for (int i = 0; i < arr.length; i++) {
            ans ^= arr[i];
        }
        System.out.println(ans);
    }

    // 有两种数出现奇数次
    public static void printOddTimesNum2(int[] arr) {
        if (arr == null || arr.length < 2) return;

        int eor = 0, ans1 = 0;
        for (int i = 0; i < arr.length; i++) {
            eor ^= arr[i];
        }

        int mostRightOne = eor & (~eor + 1);
        for (int cur : arr) {
            if ((cur & mostRightOne) != 0) {
                ans1 ^= cur;
            }
        }
        System.out.println("First: " + ans1 + ", Second: " + (ans1 ^ eor));
    }

    public static void main(String[] args) {
        int[] arr1 = { 3, 3, 2, 3, 1, 1, 1, 3, 1, 1, 1 };
        printOddTimesNum1(arr1);

        int[] arr2 = { 4, 3, 4, 2, 2, 2, 4, 1, 1, 1, 3, 3, 1, 1, 1, 4, 2, 2 };
        printOddTimesNum2(arr2);
    }
}

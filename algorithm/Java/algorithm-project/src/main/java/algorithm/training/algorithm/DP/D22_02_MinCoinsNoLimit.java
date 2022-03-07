package algorithm.training.algorithm.DP;

/**
 * @brief:      arr是面值数组，其中的值都是正数且没有重复。再给定一个正数aim。
 *              每个值都认为是一种面值，且认为张数是无限的。
 *              返回组成aim的最少货币数
 * */
public class D22_02_MinCoinsNoLimit {
    public static int minCoins(int[] arr, int aim) {
        return 0;
    }

    public static int minCoins1(int[] arr, int aim) {
        return 0;
    }

    public static int minCoins2(int[] arr, int aim) {
        return 0;
    }

    /*************************** For Test ***********************/
    public static int[] randomArray(int maxLen, int maxValue) {
        int N = (int) (Math.random() * maxLen);
        int[] arr = new int[N];
        boolean[] has = new boolean[maxValue + 1];
        for (int i = 0; i < N; i++) {
            do {
                arr[i] = (int) (Math.random() * maxValue) + 1;
            } while (has[arr[i]]);
            has[arr[i]] = true;
        }
        return arr;
    }

    public static void printArray(int[] arr) {
        for (int i = 0; i < arr.length; i++) {
            System.out.print(arr[i] + " ");
        }
        System.out.println();
    }

    public static void main(String[] args) {
        int maxLen = 20;
        int maxValue = 30;
        int testTime = 300000;
        System.out.println("功能测试开始");
        for (int i = 0; i < testTime; i++) {
            int N = (int) (Math.random() * maxLen);
            int[] arr = randomArray(N, maxValue);
            int aim = (int) (Math.random() * maxValue);
            int ans1 = minCoins(arr, aim);
            int ans2 = minCoins1(arr, aim);
            int ans3 = minCoins2(arr, aim);
            if (ans1 != ans2 || ans1 != ans3) {
                System.out.println("Oops!");
                printArray(arr);
                System.out.println(aim);
                System.out.println(ans1);
                System.out.println(ans2);
                break;
            }
        }
        System.out.println("功能测试结束");
    }
}

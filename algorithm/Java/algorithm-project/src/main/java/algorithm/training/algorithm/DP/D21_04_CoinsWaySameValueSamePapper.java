package algorithm.training.algorithm.DP;

/**
 * @brief:      arr是货币数组，其中的值都是正数。再给定一个正数aim。
 *              每个值都认为是一张货币，认为值相同的货币没有任何不同，
 *              返回组成aim的方法数
 *              例如：arr = {1,2,1,1,2,1,2}，aim = 4
 *              方法：1+1+1+1、1+1+2、2+2
 *              一共就3种方法，所以返回3
 * */
public class D21_04_CoinsWaySameValueSamePapper {
    public static int coinsWay(int[] arr, int aim) {
        return 0;
    }

    public static int coinsWay1(int[] arr, int aim) {
        return 0;
    }

    public static int coinsWay2(int[] arr, int aim) {
        return 0;
    }

    /**************************** For Test *************************/
    public static int[] randomArray(int maxLen, int maxValue) {
        int N = (int) (Math.random() * maxLen);
        int[] arr = new int[N];
        for (int i = 0; i < N; i++) {
            arr[i] = (int) (Math.random() * maxValue) + 1;
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
        int maxLen = 10;
        int maxValue = 20;
        int testTime = 1000000;
        System.out.println("测试开始");
        for (int i = 0; i < testTime; i++) {
            int[] arr = randomArray(maxLen, maxValue);
            int aim = (int) (Math.random() * maxValue);
            int ans1 = coinsWay(arr, aim);
            int ans2 = coinsWay1(arr, aim);
            int ans3 = coinsWay2(arr, aim);
            if (ans1 != ans2 || ans1 != ans3) {
                System.out.println("Oops!");
                printArray(arr);
                System.out.println(aim);
                System.out.println(ans1);
                System.out.println(ans2);
                System.out.println(ans3);
                break;
            }
        }
        System.out.println("测试结束");
    }
}

package algorithm.training.algorithm.DP;

/**
 * @brief:      给定一个正数数组arr，请把arr中所有的数分成两个集合
 *              如果arr长度为偶数，两个集合包含数的个数要一样多
 *              如果arr长度为奇数，两个集合包含数的个数必须只差一个
 *              请尽量让两个集合的累加和接近
 *              返回最接近的情况下，较小集合的累加和
 * */
public class D23_02_SplitSumClosedSizeHalf {
    public static int right(int[] arr) {
        return 0;
    }

    public static int dp1(int[] arr) {
        return 0;
    }

    public static int dp2(int[] arr) {
        return 0;
    }

    /*************** For Test *********************/
    public static int[] randomArray(int len, int value) {
        int[] arr = new int[len];
        for (int i = 0; i < arr.length; i++) {
            arr[i] = (int) (Math.random() * value);
        }
        return arr;
    }

    public static void printArray(int[] arr) {
        for (int num : arr) {
            System.out.print(num + " ");
        }
        System.out.println();
    }

    public static void main(String[] args) {
        int maxLen = 10;
        int maxValue = 50;
        int testTime = 10000;
        System.out.println("测试开始");
        for (int i = 0; i < testTime; i++) {
            int len = (int) (Math.random() * maxLen);
            int[] arr = randomArray(len, maxValue);
            int ans1 = right(arr);
            int ans2 = dp1(arr);
            int ans3 = dp2(arr);
            if (ans1 != ans2 || ans1 != ans3) {
                printArray(arr);
                System.out.println(ans1);
                System.out.println(ans2);
                System.out.println(ans3);
                System.out.println("Oops!");
                break;
            }
        }
        System.out.println("测试结束");
    }
}

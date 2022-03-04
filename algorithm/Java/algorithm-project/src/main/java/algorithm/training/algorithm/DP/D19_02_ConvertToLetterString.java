package algorithm.training.algorithm.DP;

/**
 * @brief:      规定1和A对应、2和B对应、3和C对应...26和Z对应
 *              那么一个数字字符串比如"111”就可以转化为:
 *              "AAA"、"KA"和"AK"
 *              给定一个只有数字字符组成的字符串str，返回有多少种转化结果
 * */
public class D19_02_ConvertToLetterString {
    public static int number(String str) {
        if (str == null || str.length() == 0) {
            return 0;
        }
        return process(str.toCharArray(), 0);
    }

    public static int process(char[] arr, int index) {
        if (index == arr.length) return 1;
        if (arr[index] == '0') return 0;

        int ways = process(arr, index + 1);
        if (index + 1 < arr.length &&
                (arr[index] - '0') * 10 + (arr[index + 1] - '0') < 27) {
            ways += process(arr, index+2);
        }
        return ways;
    }

    public static int number2(String s) {
        if (s == null || s.length() == 0) {
            return 0;
        }

        int N = s.length();
        int[] dp = new int[N+1];
        dp[N] = 1;
        char[] str = s.toCharArray();
        for (int i = N - 1; i >= 0; i--) {
            if (str[i] != '0') {
                dp[i] = dp[i + 1];
                if (i + 1 < N && ((str[i] - '0') * 10 + (str[i + 1] - '0')) < 27) {
                    dp[i] += dp[i + 2];
                }
            }
        }
        return dp[0];
    }
    /********************** For Test ****************************/
    public static String randomString(int len) {
        char[] str = new char[len];
        for (int i = 0; i < len; i++) {
            str[i] = (char) ((int) (Math.random() * 10) + '0');
        }
        return String.valueOf(str);
    }

    public static void main(String[] args) {
        int N = 30;
        int testTime = 1000000;
        System.out.println("测试开始");
        for (int i = 0; i < testTime; i++) {
            int len = (int) (Math.random() * N);
            String s = randomString(len);
            int ans0 = number(s);
            int ans2 = number2(s);
            if (ans0 != ans2) {
                System.out.println(s);
                System.out.println(ans0);
                System.out.println(ans2);
                System.out.println("Oops!");
                break;
            }
        }
        System.out.println("测试结束");
    }
}

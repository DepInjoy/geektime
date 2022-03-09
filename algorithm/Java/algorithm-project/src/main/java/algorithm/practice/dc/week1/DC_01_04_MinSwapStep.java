package algorithm.practice.dc.week1;
/**
 * @brief:      一个数组中只有两种字符'G'和’B’，
 *              可以让所有的G都放在左侧，所有的B都放在右侧
 *              或者可以让所有的G都放在右侧，所有的B都放在左侧
 *              但是只能在相邻字符之间进行交换操作，
 *              返回至少需要交换几次
 * */
public class DC_01_04_MinSwapStep {
    public static int minSteps1(String s) {
        return 0;
    }

    public static int minSteps2(String s) {
        return 0;
    }
    /******************** For Test *******************/
    public static String randomString(int maxLen) {
        char[] str = new char[(int) (Math.random() * maxLen)];
        for (int i = 0; i < str.length; i++) {
            str[i] = Math.random() < 0.5 ? 'G' : 'B';
        }
        return String.valueOf(str);
    }

    public static void main(String[] args) {
        int maxLen = 100;
        int testTime = 1000000;
        System.out.println("测试开始");
        for (int i = 0; i < testTime; i++) {
            String str = randomString(maxLen);
            int ans1 = minSteps1(str);
            int ans2 = minSteps2(str);
            if (ans1 != ans2) {
                System.out.println("Oops!");
            }
        }
        System.out.println("测试结束");
    }
}


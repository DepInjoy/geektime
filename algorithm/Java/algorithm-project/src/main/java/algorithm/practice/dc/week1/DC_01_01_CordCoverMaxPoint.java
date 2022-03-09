package algorithm.practice.dc.week1;

import java.util.Arrays;

/**
 * @breif:      给定一个有序数组arr，代表坐落在X轴上的点
 *              给定一个正数K，代表绳子的长度
 *              返回绳子最多压中几个点？即使绳子边缘处盖住点也算盖住
 * */
public class DC_01_01_CordCoverMaxPoint {
    public static int maxPoint1(int[] arr, int L) {
        return 0;
    }

    public static int maxPoint2(int[] arr, int L) {
        return 0;
    }

    /******************** For Test *******************/
    public static int test(int[] arr, int L) {
        int max = 0;
        for (int i = 0; i < arr.length; i++) {
            int pre = i - 1;
            while (pre >= 0 && arr[i] - arr[pre] <= L) {
                pre--;
            }
            max = Math.max(max, i - pre);
        }
        return max;
    }

    public static int[] generateArray(int len, int max) {
        int[] ans = new int[(int) (Math.random() * len) + 1];
        for (int i = 0; i < ans.length; i++) {
            ans[i] = (int) (Math.random() * max);
        }
        Arrays.sort(ans);
        return ans;
    }

    public static void main(String[] args) {
        int len = 100;
        int max = 1000;
        int testTime = 100000;
        System.out.println("测试开始");
        for (int i = 0; i < testTime; i++) {
            int L = (int) (Math.random() * max);
            int[] arr = generateArray(len, max);
            int ans1 = maxPoint1(arr, L);
            int ans2 = maxPoint2(arr, L);
            int ans3 = test(arr, L);
            if (ans1 != ans2 || ans2 != ans3) {
                System.out.println("oops!");
                break;
            }
        }
    }
}

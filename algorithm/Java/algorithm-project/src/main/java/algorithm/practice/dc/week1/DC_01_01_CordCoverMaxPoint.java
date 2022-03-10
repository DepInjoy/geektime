package algorithm.practice.dc.week1;

import java.util.Arrays;

/**
 * @breif:      给定一个有序数组arr，代表坐落在X轴上的点
 *              给定一个正数K，代表绳子的长度
 *              返回绳子最多压中几个点？即使绳子边缘处盖住点也算盖住
 *
 *              实现思想
 *                  窗口函数
 *                  建立l和r两个index,当arr[r]-arr[l]<=绳子长度，则r++,否则l++
 * */
public class DC_01_01_CordCoverMaxPoint {
    public static int maxPoint(int[] arr, int L) {
        if(arr == null || arr.length == 0) return 0;

        int l = 0, r = 0;
        int ans = 0;
        while (l < arr.length) {
            while (r < arr.length && arr[r] - arr[l] <= L) {
                r++;
            }
            ans = Math.max(ans, r - l++);
        }
        return ans;
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
            int ans1 = maxPoint(arr, L);
            int ans3 = test(arr, L);
            if (ans1 != ans3) {
                System.out.println("oops!");
                break;
            }
        }
        System.out.println("测试结束");
    }
}

package algorithm.practice.dc.week3;

import java.util.Arrays;

/**
 * @brief   给定一个正数数组arr，代表若干人的体重, 再给定一个正数limit,表示所有船共同拥有的载重量
 *          每艘船最多坐两人，且不能超过载重, 想让所有的人同时过河,并且用最好的分配方法让船尽量少
 *          返回最少的船数
 */

public class L03_05_MinBoat {
    public static int minBoat(int[] arr, int limit) {
        if (arr == null || arr.length < 2) return 0;

        Arrays.sort(arr);
        if (arr[0] >= limit) return 0;

        int left = 0, right = arr.length - 1;
        int N = arr.length;
        int lessR = -1;
        int ans = 0;
        if (arr[N-1] > limit/2) {
            while (left <= right) {
                int mid = (left + right) >> 1;
                if (arr[mid] <= limit / 2) {
                    if (arr[mid + 1] > limit / 2) {
                        lessR = mid;
                        break;
                    }
                    left = mid + 1;
                } else {
                    right = mid + 1;
                }
            }
        } else {
            lessR = N - 1;
        }

        int li = lessR - 1, ri = lessR;
        while (li >= 0 && ri < N && arr[li--] + arr[ri++] <= limit) {
            ans++;
        }
        if(li > 0) ans += (li + 1) >> 1;
        return ans;
    }

    public static void main(String[] args) {
        int[] arr0 = {0, 1, 3, 4, 5, 6, 7, 8};
        int limit = 6;
        assert minBoat(arr0, limit) == 2;

        int[] arr1 = {0, 1, 3, 4, 5, 6, 7, 8};
        limit = 16;
        assert minBoat(arr0, limit) == 4;
    }

}

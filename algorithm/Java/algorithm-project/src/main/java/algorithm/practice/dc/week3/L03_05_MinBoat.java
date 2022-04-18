package algorithm.practice.dc.week3;

import java.util.Arrays;

/**
 * @brief   给定一个正数数组arr，代表若干人的体重, 再给定一个正数limit,表示所有船共同拥有的载重量
 *          每艘船最多坐两人，且不能超过载重, 想让所有的人同时过河,并且用最好的分配方法让船尽量少
 *          返回最少的船数
 *
 *          实现思想:
 *              1. 暴力递归实现:全排列然后前后两两凑对，凑不成一对的单独一船，对所有的排列取最小值
 *              2. 将arr进行排序，找到小于等于limit/2的位置mostLessR，利用窗口进行配对
 *                  初始化，L=mostLessR， R=mostLessR+1
 *                  未配对的人数 unresoled=0
 *                  while(L >= 0 && R < N) {
 *                      if (arr[L] + arr[R] <= limit) { L++, R++, ans++;}
 *                      if (arr[L] + arr[R] > limit) {L--, unresoled++}
 *                  }
 *
 *                  已经配对好的人的对数有: ans
 *                  需要单独一船的人数有：N - R
 *                  体重小于limit/2两人一船的人数:(unresoled + L + 1 + 1)/2，需要向上取整
 */

public class L03_05_MinBoat {
    // 暴力递归，获取数组的全排列，进行一一配对，找到最少的船数
    public static int minBoat1(int[] arr, int limit) {
        if (arr == null || arr.length == 0) return 0;

        return process(arr, 0, limit);
    }

    public static int process(int[] arr, int index, int limit) {
        int ans = Integer.MAX_VALUE;
        if (index == arr.length) {
            ans = 0;
            for (int i = 0; i < arr.length;) {
                if (i == arr.length - 1) {
                    return arr[i] <= limit ? ans + 1 : -1;
                }

                if (arr[i] > limit || arr[i+1] > limit)
                    return -1;

                if (arr[i] + arr[i + 1] <= limit) {
                    ans++;
                    i += 2;
                } else {
                    if (arr[i++] <= limit) {
                        ans++;
                    }
                }
            }
            return ans;
        }

        for (int i = index; i < arr.length; i++) {
            swap(arr, index, i);
            ans = Math.min(ans, process(arr, index + 1, limit));
            swap(arr, index, i);
        }
        return ans;
    }

    public static void swap(int[] arr, int i1, int i2) {
        int tmp = arr[i1];
        arr[i1] = arr[i2];
        arr[i2] = tmp;
    }

    public static int getLessMostR(int[] arr, int limit) {
        int N = arr.length;
        int left = 0, right = N - 1;
        int ans = 0;
        while (left <= right) {
            int mid = (left + right) >> 1;
            if (arr[mid] <= limit) {
                left = mid + 1;
                ans = mid;
            } else {
                right = mid - 1;
            }
        }
        return ans;
    }

    public static int minBoat2(int[] arr, int limit) {
        if (arr == null || arr.length == 0) return 0;

        Arrays.sort(arr);
        int N = arr.length;
        if (arr[N - 1] > limit) return -1;

        int ans = 0, unresoled = 0;
        int mostLessR = getLessMostR(arr, limit/2);
        int L = mostLessR, R = mostLessR + 1;
        while (L >= 0 && R < N) {
            if (arr[L] + arr[R] <= limit) {
                ans++;
                L--;
                R++;
            } else if(arr[L] + arr[R] > limit) {
                unresoled++;
                L--;
            }
        }
        // 小于limit/2的数量为unresoled+L+1,需要的小船的数量需要向上取整
        ans += ((unresoled + L + 2) >> 1) + (N - R);
        return ans;
    }

    /*********************** For Test******************/
    public static int[] randomArray(int n, int valueMax) {
        int[] ans = new int[n];
        for (int i = 0; i < n; i++) {
            ans[i] = (int) (Math.random() * valueMax) + 1;
        }
        return ans;
    }

    public static int[] copyArray(int[] arr) {
        int N = arr.length;
        int[] ans = new int[N];
        for (int i = 0; i < N; i++) {
            ans[i] = arr[i];
        }
        return ans;
    }

    public static void printInput(int[] x) {
        System.out.print("input: ");
        for (int i = 0; i < x.length; i++) {
            System.out.print(x[i] + " ");
        }
        System.out.println();
    }

    public static void main(String[] args) {
        int maxValue = 20;
        int maxLen = 10;
        int maxLimit = 20;
        int testTimes = 100000;

        System.out.println("测试开始");
        for (int i = 0; i < testTimes; i++) {
            int len = (int)(Math.random() * maxLen) + 1;
            int[] arr0 = randomArray(len, maxValue);
            int[] arr1 = copyArray(arr0);
            int limit = (int)(Math.random() * maxLimit) + 1;
            int ans1 = minBoat1(arr0, limit);
            int ans2 = minBoat2(arr1, limit);
            if (ans1 != ans2) {
                System.out.println("Opps, Error");
                printInput(arr0);
                System.out.println("limit=" + limit);
                System.out.println("ans1=" + ans1);
                System.out.println("ans2=" + ans2);
                break;
            }
        }
        System.out.println("测试结束");
        int[] arr0 = {11, 10, 5, 16, 11};//0, 1, 3, 4, 5, 6, 7, 8};
        int limit = 19;//10;
        int ans1 = minBoat1(arr0, limit);
        int ans2 = minBoat2(arr0, limit);
        System.out.println(ans1 + " " + ans2);


        int[] arr1 = {0, 1, 3, 4, 5, 6, 7, 8};
        limit = 16;
        ans1 = minBoat1(arr1, limit);
        ans2 = minBoat2(arr1, limit);
        System.out.println(ans1 + " " + ans2);
    }

}

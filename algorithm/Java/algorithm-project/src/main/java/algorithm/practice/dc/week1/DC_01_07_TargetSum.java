package algorithm.practice.dc.week1;

import java.util.HashMap;

/**
 *  @brief:     leetcode 494题:
 *                  给定一个数组arr, 你可以在每个数字之前决定+或者-
 *                  但是必须所有数字都参与, 再给定一个数target
 *                  返回最后算出target的方法数是多少？
 *
 *             实现思路：
 *                  1.暴力递归，在index位置,计算出rest的方法数(按照要index和不要index两种)
 *                  2. 记忆化搜索+动态规划,由于rest的取值范围很大,采取HashMap进行数据缓存
 *                      <index, <rest, 计算出rest的方法数>>
 *                  3. 将输入全部按照正数处理,其中全部正数的和为sum,计算从数组中取数据得到和为(sum+target)/2的方法数
 *                     记添加+的子序列的序列P，添加-的序列为Q，则有:
 *                              P + Q = Target
 *                              P - Q = sum
 *                          2P = target + sum
 *                          P = (target + sum)/2
 *                    因而，该问题可以转化为从输入都为正数的序列中查找和为(target + sum)/2的方法数
 *                    同时应该满足sum>target且sum和target和sum的奇偶性相同
 * */
public class DC_01_07_TargetSum {
    // 暴力递归
    public int findTargetSumWays1(int[] arr, int s) {
        if (arr == null || arr.length == 0) return 0;

        return process(arr, 0, s);
    }

    // 计算在index位置,计算出rest的方法数
    public int process(int[] arr, int index, int rest) {
        if (index == arr.length) return rest == 0 ? 1 : 0;
        return process(arr, index + 1, rest - arr[index]) +
                process(arr, index + 1, rest + arr[index]);
    }

    // 动态规划+记忆化搜索
    public int findTargetSumWays2(int[] arr, int s) {
        if (arr == null || arr.length == 0) return 0;

        HashMap<Integer, HashMap<Integer, Integer>> dp = new HashMap<Integer, HashMap<Integer, Integer>>();
        return process(arr, 0, s, dp);
    }

    public int process(int[] arr, int index, int rest, HashMap<Integer, HashMap<Integer, Integer>> dp) {
        if (dp.containsKey(index) && dp.get(index).containsKey(rest)) {
            return dp.get(index).get(rest);
        }

        int ans = 0;
        if (index == arr.length) {
            ans = (rest == 0 ? 1 : 0);
        } else {
            ans = process(arr, index + 1, rest - arr[index], dp) +
                    process(arr, index + 1, rest + arr[index], dp);
        }

        if (!dp.containsKey(index)) {
            dp.put(index, new HashMap<>());
        }
        dp.get(index).put(rest, ans);
        return ans;
    }

    // 将所有的数据当成正数处理
    public int findTargetSumWays3(int[] arr, int s) {
        if (arr == null || arr.length == 0) return 0;

        int sum = 0;
        for (int i = 0; i < arr.length; i++) {
            if (arr[i] < 0) {
                sum -= arr[i];
            } else {
                sum += arr[i];
            }
        }
        return (sum < s) ? 0 : (((sum & 1) != (s & 1)) ? 0 : subset(arr, (sum + s) >> 1));
    }

    // 计算从都是正数的数组中取数据得到和为(sum+target)/2的方法数
    public int subset(int[] arr, int target) {
        if (target < 0) return 0;

        int N = arr.length;
        int[][] dp = new int[N+1][target+1];
        dp[N][0] = 1;
        for (int i = N-1; i >= 0; i--) {
            for (int rest = target; rest >= 0; rest--) {
                dp[i][rest] = dp[i+1][rest];
                int tmp = arr[i] > 0 ? arr[i] : -arr[i];
                if (rest >= tmp) {
                    dp[i][rest] += dp[i+1][rest - tmp];
                }
            }
        }
        return dp[0][target];
    }

    // 空间压缩
    public int subset2(int[] arr, int target) {
        if (target < 0) return 0;

        int N = arr.length;
        int[] dp = new int[target+1];
        dp[0] = 1;
        for (int i = N-1; i >= 0; i--) {
            for (int rest = target; rest >= 0; rest--) {
                int tmp = arr[i] > 0 ? arr[i] : -arr[i];
                if (rest >= tmp) {
                    dp[rest] += dp[rest - tmp];
                }
            }
        }
        return dp[target];
    }
}

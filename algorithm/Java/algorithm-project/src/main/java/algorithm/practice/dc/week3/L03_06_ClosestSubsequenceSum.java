package algorithm.practice.dc.week3;

import java.lang.reflect.Array;
import java.util.Arrays;

/**
 * @brief:  Leetcode 最接近sum的子序列累加和问题
 *          https://leetcode-cn.com/problems/closest-subsequence-sum/
 * */
public class L03_06_ClosestSubsequenceSum {
    public int minAbsDifference(int[] nums, int goal) {
        if (nums == null || nums.length == 0) return -1;

        int[] lSubSum = new int[1 << 20];
        int[] rSubSum = new int[1 << 20];
        int lend = process(nums, 0, nums.length >> 1, 0, 0, lSubSum);
        int rend = process(nums, nums.length >> 1, nums.length, 0, 0, rSubSum);
        Arrays.sort(lSubSum, 0, lend);
        Arrays.sort(rSubSum, 0, rend--);
        int ans = Math.abs(goal);
        for(int i = 0; i < lend; i++) {
            int rest = goal - lSubSum[i];
            while (rend > 0 && Math.abs(rest-rSubSum[rend-1]) <= Math.abs(rest-rSubSum[rend])) rend--;
            ans = Math.min(ans, Math.abs(rest - rSubSum[rend]));
        }
        return ans;
    }

    public int process(int[] nums, int index, int end, int fillIndex, int sum, int[] arr) {
        if (index == end) {
            arr[fillIndex++] = sum;
        } else {
            fillIndex = process(nums, index + 1, end, fillIndex, sum, arr);
            fillIndex = process(nums, index + 1, end, fillIndex, nums[index] + sum, arr);
        }
        return fillIndex;
    }
}

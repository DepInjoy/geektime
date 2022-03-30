package algorithm.training.algorithm.BitOperation;

import java.util.HashMap;
import java.util.HashSet;

/**
 * @brief:      一个数组中有一种数出现K次，其他数都出现了M次，M > 1,  K < M
 *              找到，出现了K次的数，
 *              要求，额外空间复杂度O(1)，时间复杂度O(N)
 *
 *              实现思想
 *                  if (target != 0) {
 *                      对每个位置1的个数记为counts[i], i = 0....31
 *                      counts[i]可能为0, K+M, M
 *                      (counts[i] % M != 0) && (counts[i] % M != K) {
 *                          ans != 1 << i; // 恢复出数据
 *                      }
 *                  } else {
 *                      计算数组中0的个数，如果是k,则结果为0，否则为-1
 *                  }
 * */
public class D02_03_KM {
    public static int onlyKTimes(int[] arr, int k, int m) {
        if (arr == null || arr.length < 2) return -1;

        int ans = 0;
        HashMap<Integer, Integer> hashMap = new HashMap<>();
        int value = 1;
        // 建立位和数值之间的映射关系，例如1 -> 2(10)
        for (int bit = 0; bit < 32; bit++) {
            hashMap.put(value, bit);
            value <<= 1;
        }

        // 统计各位上1的个数
        int[] counts = new int[32];
        for (int cur : arr) {
            while (cur != 0) {
                int mostRightOne = cur & (~cur + 1);
                counts[hashMap.get(mostRightOne)]++;
                cur ^= mostRightOne;
            }
        }

        // 根据各位上1的个数获取结果非0情况下的结果
        for (int i = 0; i < 32; i++) {
            if (counts[i] % m != 0) {
                if (counts[i] % m == k) {
                    ans |= 1 << i;
                } else {
                    return -1;
                }
            }
        }

        // 结果为0的处理
        if (ans == 0) {
            int zeroCnt = 0;
            for (int cur : arr) {
                if (cur == 0) zeroCnt++;
            }
            if (zeroCnt != k) return -1;
        }
        return ans;
    }

    // For Test
    public static int test(int[] arr, int k, int m) {
        HashMap<Integer, Integer> hashMap = new HashMap<>();
        for (int cur : arr) {
            if (hashMap.containsKey(cur)) {
                hashMap.put(cur, hashMap.get(cur) + 1);
            } else {
                hashMap.put(cur, 1);
            }
        }

        for (int key : hashMap.keySet()) {
            if (hashMap.get(key) == k) {
                return key;
            }
        }
        return -1;
    }

    public static int[] randomArray(int maxKinds, int range, int k, int m) {
        int ktimeNum = randomNumber(range);
        int times = Math.random() < 0.5 ? k : ((int) (Math.random() * (m - 1)) + 1);
        int numKinds = (int) (Math.random() * maxKinds) + 2;
        int[] arr = new int[times + (numKinds - 1) * m]; // k * 1 + (numKinds - 1) * m
        int index = 0;
        for (; index < times; index++) {
            arr[index] = ktimeNum;
        }
        numKinds--;
        HashSet<Integer> set = new HashSet<>();
        set.add(ktimeNum);
        while (numKinds != 0) {
            int curNum = 0;
            do {
                curNum = randomNumber(range);
            } while (set.contains(curNum));
            set.add(curNum);
            numKinds--;
            for (int i = 0; i < m; i++) {
                arr[index++] = curNum;
            }
        }
        // arr 填好了
        for (int i = 0; i < arr.length; i++) {
            // i 位置的数，我想随机和j位置的数做交换
            int j = (int) (Math.random() * arr.length);// 0 ~ N-1
            int tmp = arr[i];
            arr[i] = arr[j];
            arr[j] = tmp;
        }
        return arr;
    }

    // [-range, +range]
    public static int randomNumber(int range) {
        return ((int) (Math.random() * range) + 1) - ((int) (Math.random() * range) + 1);
    }

    public static void main(String[] args) {
        int kinds = 5;
        int range = 30;
        int testTime = 100000;
        int max = 9;
        System.out.println("测试开始");
        for (int i = 0; i < testTime; i++) {
            int a = (int) (Math.random() * max) + 1; // a 1 ~ 9
            int b = (int) (Math.random() * max) + 1; // b 1 ~ 9
            int k = Math.min(a, b);
            int m = Math.max(a, b);
            if (k == m) {  // k < m
                m++;
            }
            int[] arr = randomArray(kinds, range, k, m);
            int ans1 = test(arr, k, m);
            int ans2 = onlyKTimes(arr, k, m);
            if (ans1 != ans2) {
                for (int item : arr) {
                    System.out.print(item + " ");
                }
                System.out.println();
                System.out.println(ans1);
                System.out.println(ans2);
                System.out.println("出错了！");
            }
        }
        System.out.println("测试结束");
    }
}

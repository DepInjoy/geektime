package algorithm.practice.dc.week3;

import java.util.HashSet;

/**
 * @brief       只由小写字母（a~z）组成的一批字符串,都放在字符类型的数组String[] arr中
 *              如果其中某两个字符串所含有的字符种类完全一样,就将两个字符串算作一类
 *              比如：baacbba和bac就算作一类
 *              返回arr中有多少类？
 *
 *              实现思想：
 *                  方式1: 只有26个字符，用一个26个bool进行记录出现的位置记为true，
 *                          之后将true的位置转换为字符串，并添加到HashSet中，返回size即可
 *                  方式2：int有32位，利用1到26位出现字符的位置记录为1
 *                          将数据添加到HashSet返回size
 */

public class L03_02_HowManyTypes {
    public static int types(String[] arr) {
        if (arr == null || arr.length == 0) return 0;

        HashSet<String> hashSet = new HashSet<>();
        for (String str : arr) {
            char[] chars = str.toCharArray();
            boolean[] exists = new boolean[26];
            for (char ch : chars) {
                exists[ch - 'a'] = true;
            }

            String unique = "";
            for (int i = 0; i < 26; i++) {
                if (exists[i]) {
                    unique += String.valueOf(i + 'a');
                }
            }
            hashSet.add(unique);
        }
        return hashSet.size();
    }

    public static int types2(String[] arr) {
        if (arr == null || arr.length == 0) return 0;

        HashSet<Integer> hashSet = new HashSet<>();
        for (String str : arr) {
            int exist = 0;
            char[] chars = str.toCharArray();
            for (char ch : chars) {
                exist |= (1 << (ch - 'a'));
            }
            hashSet.add(exist);
        }
        return hashSet.size();
    }

    public static String[] getRandomStringArray(
            int possibilities,int strMaxSize, int arrMaxSize) {
        String[] ans = new String[(int) (Math.random() * arrMaxSize) + 1];
        for (int i = 0; i < ans.length; i++) {
            ans[i] = getRandomString(possibilities, strMaxSize);
        }
        return ans;
    }

    /************************* For test *********************/
    public static String getRandomString(int possibilities, int strMaxSize) {
        char[] ans = new char[(int) (Math.random() * strMaxSize) + 1];
        for (int i = 0; i < ans.length; i++) {
            ans[i] = (char) ((int) (Math.random() * possibilities) + 'a');
        }
        return String.valueOf(ans);
    }

    public static void print(String[] arr) {
        if (arr != null) {
            for (String str : arr) {
                System.out.print(str + " ");
            }
            System.out.println();
        }
    }

    public static void main(String[] args) {
        int possibilities = 5;
        int strMaxSize = 10;
        int arrMaxSize = 100;
        int testTimes = 500000;
        System.out.println("test begin, test time : " + testTimes);
        for (int i = 0; i < testTimes; i++) {
            String[] arr = getRandomStringArray(possibilities, strMaxSize, arrMaxSize);
            int ans1 = types(arr);
            int ans2 = types2(arr);
            if (ans1 != ans2) {
                System.out.println("Oops!");
                print(arr);
                break;
            }
        }
        System.out.println("test finish");
    }
}

package algorithm.training.algorithm.kmp;

/**
 * @brief:                  KMP算法
 *              假设字符串str长度为N，字符串match长度为M，M <= N
 *              确定str中是否有某个子串是等于match
 *
 *              时间复杂度O(N)
 *
 *              实现思路：
 *                  1. 构建next数组
 *                  2. 利用next数组加速匹配过程
 * */
public class D27_01_KMP {
    public static int getIndexOf(String s1, String s2) {
        if (s1 == null || s2 == null || s1.length() == 0 || s2.length() == 0 || s1.length() < s2.length())
            return -1;

        char[] str1 = s1.toCharArray();
        char[] str2 = s2.toCharArray();
        int [] next = getNextArray(str2);
        int x = 0, y = 0;
        while (x < str1.length && y < str2.length) {
            if (str1[x] == str2[y]) {
                x++;
                y++;
            } else if (next[y] == -1) { // 即y=0
                x++;
            } else {
                y = next[y];
            }
        }
        return y == str2.length ? x - y : -1;
    }

    // 生成next数组,时间复杂度为O(M),M是S的长度
    public static int[] getNextArray(char[] s) {
        if (s.length == 1) {
            return new int[]{-1};
        }

        int[] next = new int[s.length];
        next[0] = -1;
        next[1] = 0;
        int i = 2;
        int cnt = 0; // 前缀和后缀的匹配长度
        while (i < s.length) {
            if (s[cnt] == s[i - 1]) {  // 匹配成功
                next[i++] = ++cnt;
            } else if (cnt > 0) { // 存在匹配的前缀则跳到匹配位置
                cnt= next[cnt];
            } else {
                next[i++] = 0;
            }
        }
        return next;
    }

    /** --------------------------- For Test ------------------------------ **/
    public static String getRandomString(int possibilities, int size) {
        char[] ans = new char[(int) (Math.random() * size) + 1];
        for (int i = 0; i < ans.length; i++) {
            ans[i] = (char) ((int) (Math.random() * possibilities) + 'a');
        }
        return String.valueOf(ans);
    }

    public static void main(String[] args) {
        int possibilities = 5;
        int strSize = 20;
        int matchSize = 5;
        int testTimes = 5000000;
        System.out.println("test begin");
        for (int i = 0; i < testTimes; i++) {
            String str = getRandomString(possibilities, strSize);
            String match = getRandomString(possibilities, matchSize);
            if (getIndexOf(str, match) != str.indexOf(match)) {
                System.out.println("Oops!");
            }
        }
        System.out.println("test finish");
    }
}

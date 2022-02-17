package algorithm.training.algorithm.Manacher;

public class ManacherV2 {
    // reference:https://segmentfault.com/a/1190000008484167
    public static int manacher(String s) {
        if (s == null || s.length() == 0) return 0;

        // 1. 特殊字符串处理，例如12132 -> $1#2#1#3#2^
        char[] str = manacherString(s);
        int[] pArr = new int[str.length]; // 回文半径
        int C = -1; // 回文中心
        int R = -1; // 最右扩展成功的位置的下一个位置
        int max = Integer.MIN_VALUE;
        for (int i = 0; i < str.length; ++i) {
            pArr[i] = R > i ? Math.min(pArr[2 * C - i], R - i) : 1;
            // 不需要越界判断判断，因为左有$, 右有^
            while (str[i + pArr[i]] == str[i - pArr[i]]) {
                pArr[i]++;
            }
            if (i + pArr[i] > R) {
                R = i + pArr[i];
                C = i;
            }
            max = Math.max(max, pArr[i]);
        }
        return max - 1;
    }

    // 添加#进行特殊处理
    // 开头和结尾字符分别设置为'$'和'^',达到中心扩展寻找回文的时候会自动退出循环，不需每次判断是否越界
    public static char[] manacherString(String s) {
        char[] charArr = s.toCharArray();
        char[] res = new char[2 * s.length() + 1];
        int index = 0;
        res[0] = '$';
        for (int i = 1; i != res.length - 1; i++) {
            res[i] = (i & 1) == 0 ? '#' : charArr[index++];
        }
        res[res.length - 1] = '^';
        return res;
    }

    /******************* For Test **********************/
    public static int right(String s) {
        if (s == null || s.length() == 0) {
            return 0;
        }
        char[] str = manacherString(s);
        int max = 0;
        for (int i = 0; i < str.length; i++) {
            int L = i - 1;
            int R = i + 1;
            while (L >= 0 && R < str.length && str[L] == str[R]) {
                L--;
                R++;
            }
            max = Math.max(max, R - L - 1);
        }
        return max / 2;
    }

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
        int testTimes = 5000000;
        System.out.println("test begin");
        for (int i = 0; i < testTimes; i++) {
            String str = getRandomString(possibilities, strSize);
            if (manacher(str) != right(str)) {
                System.out.println("Oops!");
            }
        }
        System.out.println("test finish");
    }
}

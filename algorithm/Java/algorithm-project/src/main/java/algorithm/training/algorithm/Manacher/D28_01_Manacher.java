package algorithm.training.algorithm.Manacher;

/**
 * @brief:      假设字符串str长度为N，返回最长回文子串的长度
 *
 *              时间复杂度O(N)
 * */
public class D28_01_Manacher {
    public static int manacher(String s) {
        if (s == null || s.length() == 0) return 0;

        // 1. 特殊字符串处理，例如12132 -> #1#2#1#3#2#
        char[] str = manacherString(s);
        int[] pArr = new int[str.length]; // 回文半径
        int C = -1; // 回文中心
        int R = -1; // 最右扩展成功的位置的下一个位置
        int max = Integer.MIN_VALUE;
        /* 2. 计算最右扩展成功的位置R，此时主要划分为两种情况：
                2.1 i没有在R范围内(R<i),不优化继续左右扩张寻找回文
                2.2 i在R的范围内(R>=i),优化,此时划分为三种情况讨论,记i以C为中心的对称点为i'
                    2.2.1 i'的回文区域在L、R的范围内，此时i的回文区域和R的回文区域相同
                    2.2.2 i'的回文区域在L的外部，此时i的回文区域为R-i
                    2.2.3 i'的回文区域正好压L,此时i的回文区域包含R-i，但R的右侧可能依然是回文区域
                          需要R依然需要继续扩充
                    2.2.1和2.2.2可以确定i的回文区域为min(pArr[i'], R-i),其中i'=2C-i
                pArr[i] = R > i ? Math.min(pArr[2 * C - i], R - i) : 1;
         */
        for (int i = 0; i < str.length; ++i) {
            pArr[i] = R > i ? Math.min(pArr[2 * C - i], R - i) : 1;
            while (i + pArr[i] < str.length && i - pArr[i] > -1 &&
                    str[i + pArr[i]] == str[i - pArr[i]]) {
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

    // 添加#进行特殊处理,避免偶回文和奇回文问题(将偶回文转换为奇回文)
    // 其中添加的特殊字符#是否属于待处理字符串, 不会影响判断, 由于实字符和虚字符不会相遇
    public static char[] manacherString(String s) {
        char[] charArr = s.toCharArray();
        char[] res = new char[2 * s.length() + 1];
        int index = 0;
        for (int i = 0; i != res.length; i++) {
            res[i] = (i & 1) == 0 ? '#' : charArr[index++];
        }
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

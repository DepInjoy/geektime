package algorithm.training.algorithm.DP;

import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;

/**
 * @brief:      打印一个字符串的全部(不重复)子序列
 *
 *              实现思路：
 *                  对于i位置上的字符，选择要/不要，递归
 *                  对于结果不需要重复的，将结果加入HashSet去重即可
 *             
 *              对于123
 *                  1
 *               √      x
 *          2               2
 *        √    x        √       x
 *      3√       3x   3√        3 x
 * */

public class D17_03_PrintAllSubsquences {
    // 打印一个字符串的全部子序列
    public static List<String> subs(String s) {
        if (s == null || s.length() == 0) return null;

        char[] str = s.toCharArray();
        List<String> ans = new ArrayList<>();
        process(str, 0, ans, "");
        return ans;
    }

    // 打印一个字符串的全部不重复子序列
    public static List<String> subsNoRepeate(String s) {
        if (s == null || s.length() == 0) return null;

        char[] str = s.toCharArray();
        HashSet<String> noRepeatedSet = new HashSet<>();
        List<String> ans = new ArrayList<>();
        process2(str, 0, noRepeatedSet, "");
        for (String item : noRepeatedSet) {
            ans.add(item);
        }
        return ans;
    }

    public static void process(char[] str, int index, List<String> ans, String path) {
        if (index == str.length) {
            ans.add(path);
            return;
        }

        // 不要index位置的字符
        process(str, index + 1, ans, path);
        // 要index位置的字符
        process(str, index + 1, ans, path + str[index]);
    }

    public static void process2(char[] str, int index, HashSet<String> ans, String path) {
        if (index == str.length) {
            ans.add(path);
            return;
        }

        // 不要index位置的字符
        process2(str, index + 1, ans, path);
        // 要index位置的字符
        process2(str, index + 1, ans, path + str[index]);
    }

    /************************* For Test *****************************/
    public static void printStrArray(List<String> arr) {
        for (String str : arr) {
            System.out.println(str);
        }
    }

    public static void main(String[] args) {
        String test0 = "acc";
        List<String> ans1 = subs(test0);
        printStrArray(ans1);

        List<String> ans2 = subsNoRepeate(test0);
        printStrArray(ans2);
    }
}

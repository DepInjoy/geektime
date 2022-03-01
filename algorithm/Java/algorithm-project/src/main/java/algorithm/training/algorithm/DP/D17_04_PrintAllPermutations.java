package algorithm.training.algorithm.DP;

import java.util.ArrayList;
import java.util.List;

/**
 * @brief:      打印一个字符串的全部排列(要求不要出现重复的排列)
 * */
public class D17_04_PrintAllPermutations {
    // 打印一个字符串的全排列
    public static List<String> permutation1(String s) {
        List<String> ans = new ArrayList<>();
        if (s == null || s.length() == 0) return ans;

        char[] str = s.toCharArray();
        process1(str,0, ans);
        return ans;
    }

    // 打印一个字符串的全排列
    public static List<String> permutation2(String s) {
        List<String> ans = new ArrayList<>();
        if (s == null || s.length() == 0) return ans;

        ArrayList<Character> rest = new ArrayList<>();
        for (char ch : s.toCharArray()) {
            rest.add(ch);
        }
        process2(rest, ans, "");
        return ans;
    }

    // 打印一个字符串的不出现重复的全部排列
    public static List<String> permutation3NoReapate(String s) {
        List<String> ans = new ArrayList<>();
        if (s == null || s.length() == 0) return ans;

        char[] str = s.toCharArray();
        process3(str, 0, ans);
        return ans;
    }

    public static void process1(char[] str, int index, List<String> ans) {
        if (index == str.length) {
            ans.add(String.valueOf(str));
            return;
        }

        for (int i = index; i < str.length; ++i) {
            swap(str, index, i);
            process1(str, index + 1, ans);
            swap(str, index, i);
        }
    }

    public static void process2(ArrayList<Character> rest, List<String> ans, String path) {
        if (rest.isEmpty()) {
            ans.add(path);
            return;
        }

        int N = rest.size();
        for (int i = 0; i < N; i++) {
            Character cur = rest.get(i);
            rest.remove(i);
            process2(rest, ans, path + String.valueOf(cur));
            rest.add(i, cur);
        }
    }

    public static void process3(char[] str, int index, List<String> ans) {
        if (index == str.length) {
            ans.add(String.valueOf(str));
            return;
        }

        boolean[] visited = new boolean[256];
        for (int i = index; i < str.length; ++i) {
            if (!visited[str[i]]) {
                visited[str[i]] = true;
                swap(str, index, i);
                process3(str, index + 1, ans);
                swap(str, index, i);
            }
        }
    }

    public static void swap(char[] str, int i, int j) {
        char tmp = str[i];
        str[i] = str[j];
        str[j] = tmp;
    }

    public static void main(String[] args) {
        String s = "acc";
        List<String> ans1 = permutation1(s);
        for (String str : ans1) {
            System.out.println(str);
        }

        System.out.println("====================");
        List<String> ans2 = permutation2(s);
        for (String str : ans2) {
            System.out.println(str);
        }


        System.out.println("====================");
        List<String> ans3 = permutation3NoReapate(s);
        for (String str : ans3) {
            System.out.println(str);
        }
    }
}

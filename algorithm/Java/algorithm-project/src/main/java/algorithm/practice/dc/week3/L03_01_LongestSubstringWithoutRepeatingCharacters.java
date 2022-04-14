package algorithm.practice.dc.week3;

/**
 * @brief       求一个字符串中，最长无重复字符子串长度
 *              https://leetcode.com/problems/longest-substring-without-repeating-characters/
 *
 *              实现思想：
 *                  最多有256个字符，构造一个数组posMap记录每个字符出现的位置，初始化为-1
 *                  从头到尾遍历整个字符串，记上一次发现的无重复字符串长度为pre
 *                  当前最长无重复字符串的长度为min(i-pos[str[i]], pre+1)
 *                      其中，i-pos[str[i]]表示当前字符到上一次出现当前字符
 *                          pre+1表示当前字符之前没有出现过
 */
public class L03_01_LongestSubstringWithoutRepeatingCharacters {
    int lengthOfLongestSubstring(String s) {
        if (s == null || s.length() == 0) return 0;

        int[] posMap = new int[256];
        for (int i = 0; i < 256; i++) {
            posMap[i] = -1;
        }

        char[] str = s.toCharArray();
        int pre = 1, ans = 1;
        for (int i = 0; i < str.length; i++) {
            pre = Math.min(i - posMap[str[i]], pre + 1);
            ans = Math.max(ans, pre);
            posMap[str[i]] = i;
        }
        return ans;
    }
}

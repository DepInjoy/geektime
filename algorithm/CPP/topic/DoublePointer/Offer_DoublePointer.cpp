#include <vector>
#include <string>
 
using namespace std;

/**
 * @brief 剑指Offer-21:调整数组顺序使奇数位于偶数前面
 *          输入一个整数数组，实现一个函数来调整该数组中数字的顺序，
 *          使得所有奇数在数组的前半部分，所有偶数在数组的后半部分。
 * 
 *          https://leetcode.cn/problems/diao-zheng-shu-zu-shun-xu-shi-qi-shu-wei-yu-ou-shu-qian-mian-lcof/description
 */
vector<int> exchange(vector<int>& nums) {
    int l = 0, r = nums.size() - 1;
    std::vector<int> ans(r+1);
    for (int num : nums) {
        if (num % 2) {
            ans[l++] = num;
        } else {
            ans[r--] = num;
        }
    }
    return ans;
}

/**
 * @brief 剑指Offer-57:和为s的两个数字
 *          输入一个递增排序的数组和一个数字s，在数组中查找两个数，使得它们的和正好是s。
 *          如果有多对数字的和等于s，则输出任意一对即可
 *          
 *          https://leetcode.cn/problems/he-wei-sde-liang-ge-shu-zi-lcof/description
 */

vector<int> twoSum(vector<int>& nums, int target) {
    std::vector<int> ans(2);
    int n = nums.size();
    int l = 0, r = n- 1;
    while(l < r) {
        if (nums[l] + nums[r] > target) {
            r--;
        } else if (nums[l] + nums[r] == target) {
            ans[0] = nums[l], ans[1] = nums[r];
            break;
        } else {
            l++;
        }
    }
    return ans;
}

/**
 * @brief 剑指 Offer 58-I:翻转单词顺序
 *          输入一个英文句子，翻转句子中单词的顺序，但单词内字符的顺序不变
 *          为简单起见，标点符号和普通字母一样处理。
 *          例如输入字符串"I am a student. "，则输出"student. a am I"。
 * 
 *          https://leetcode.cn/problems/fan-zhuan-dan-ci-shun-xu-lcof/description 
 */
string reverseWords(string s) {
    int n = s.size();
    int l = n -1, r = n -1;
    while(r >= 0 && s[r] == ' ') {
        l = --r;
    }
    if (r < 0) return "";

    int i = 0;
    std::string ans(n, '\0');
    while (r >= l && l >= 0) {
        if (s[l] == ' ') {
            for (int j = l+1; j <= r; ++j) {
                ans[i++] = s[j];
            }
            
            r = --l;
            while(l >= 0 && s[l] == ' ') {
                r = --l;
            }
            if (l + 1) ans[i++] = ' ';
        } else {
            l--;
        }
    }

    for (int j = l + 1; j <= r; ++j) {
        if (s[j] != ' ') ans[i++] = s[j];
    }
    return ans;
}
#include <vector>
#include <string>
 
using namespace std;

 struct ListNode {
     int val;
     ListNode *next;
     ListNode(int x) : val(x), next(NULL) {}
 };
 
/**
 * @brief 剑指Offer-25：合并两个排序的链表
 *          输入两个递增排序的链表，合并这两个链表并使新链表中的节点仍然是递增排序的
 * 
 *          https://leetcode.cn/problems/he-bing-liang-ge-pai-xu-de-lian-biao-lcof/description 
 */
ListNode* mergeTwoLists(ListNode* l1, ListNode* l2) {
    if (!l1 && !l2) return nullptr;

    ListNode* head = (!l2 || (l1 && l1->val <= l2->val)) ? l1 : l2;
    ListNode* cur = head;
    ListNode* c1 = head->next;
    ListNode* c2 = (head == l1) ? l2 : l1;
    while (c1 && c2) {
        if (c1->val <= c2->val) {
            cur->next = c1;
            c1 = c1->next;
        } else {
            cur->next = c2;
            c2 = c2->next;
        }
        cur = cur->next;
    }

    c1 = c1 ? c1 : c2;
    while (c1) {
        cur->next = c1;
        cur = cur->next;
        c1 = c1->next;
    }
    return head;
}

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

/**
 *  88. 合并两个有序数组
 *      https://leetcode.cn/problems/merge-sorted-array/description/
 *      给你两个按非递减顺序 排列的整数数组nums1和nums2，另有两个整数m和n,
 *      分别表示nums1和nums2中的元素数目.
 *      请你合并nums2到nums1中，使合并后的数组同样按非递减顺序排列。
*/
// 双指针, 时间和空间复杂度均为O(m+n)
void merge(vector<int>& nums1, int m, vector<int>& nums2, int n) {
    std::vector<int> ans(m+n);
    int i = 0, j = 0, k = 0;
    while ((i < m) && (j < n)) {
        if (nums1[i] <= nums2[j]) {
            ans[k++] = nums1[i++];
        } else {
            ans[k++] = nums2[j++];
        }
    }

    while (i < m) ans[k++] = nums1[i++];
    while (j < n) ans[k++] = nums2[j++];
    nums1 = ans;
}

/**
 * 逆向双指针,空间复杂度为O(1), 时间复杂度为O(m+n)
 * 
 *  在遍历过程中
 *      nums1有m-1-i个元素放入nums1的尾部
 *      nums2有n-1-j个元素放入nums2的尾部
 *      
 *      在nums1的后面有m+n-1-i个空位
 * 
 *      m+n-1-i > (m-1-i) + (n-1-j)
 * 
 *      因此不会产生覆盖
*/
void merge(vector<int>& nums1, int m, vector<int>& nums2, int n) {
    int i = m - 1, j = n- 1, k = m + n - 1;
    while ((i >= 0) && (j >= 0)) {
        if (nums1[i] >= nums2[j]) {
            nums1[k--] = nums1[i--];
        } else {
            nums1[k--] = nums2[j--];
        }
    }

    // 因为nums1本身已经有序
    // while (i >= 0) nums1[k--] = nums1[i--];
    while (j >= 0) nums1[k--] = nums2[j--];
}
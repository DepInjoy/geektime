#include <vector>
using namespace std;
/**
*   剑指 Offer II 068. 查找插入位置
*   https://leetcode.cn/problems/N6YdxV/description/
*   
*   给定一个排序的整数数组nums和一个整数目标值 target, 请在数组中找到target,
*   并返回其下标。如果目标值不存在于数组中，返回它将会被按顺序插入的位置。
*/
int searchInsert(vector<int>& nums, int target) {
    int l = 0, r= nums.size() - 1;
    int ans = nums.size();
    while (l <= r) {
        int mid = ((r - l) >> 1) + l;
        if (nums[mid] >= target) {
            ans = mid;
            r = mid - 1;
        } else {
            l = mid + 1; 
        }
    }
    return ans;
}

/**
 * 剑指 Offer II 069. 山峰数组的顶部
 *      https://leetcode.cn/problems/B1IidL/description/
 *      https://leetcode-cn.com/problems/peak-index-in-a-mountain-array/
*/
int peakIndexInMountainArray(vector<int>& arr) {
    int n = arr.size();
    int l = 0, r = n - 1;
    int ans = 0;
    while (l <= r) {
        int mid = ((r - l) >> 1) + l;
        if (arr[mid] > arr[mid+1]) {
            ans = mid;
            r = mid - 1;
        } else {
            l = mid + 1;
        }
    }
    return ans;
}

/**
 * 剑指 Offer II 070. 排序数组中只出现一次的数字
 *      https://leetcode.cn/problems/skFtm2/description/
 *      https://leetcode-cn.com/problems/single-element-in-a-sorted-array/
*/
int singleNonDuplicate(vector<int>& nums) {
    int l = 0, r = nums.size() - 1;
    int ans;
    while (l < r) {
        int mid = ((r - l) >> 1) + l;
        if (nums[mid] == nums[mid ^ 1]) {
            l = mid + 1;
        } else {
            r = mid;
        }
    }
    return nums[l];
}
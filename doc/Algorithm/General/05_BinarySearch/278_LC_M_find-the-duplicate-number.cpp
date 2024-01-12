/**
 * 287. 寻找重复数
 * https://leetcode.cn/problems/find-the-duplicate-number/description
*/

// 二分查找 时间复杂度O(NlogN), 空间复杂度为O(1)
int findDuplicate(vector<int>& nums) {
    const int n = nums.size();
    int l = 1, r = n - 1, ans = 0;
    while (l <= r) {
        int mid = l + r >> 1, cnt = 0;
        for (int i = 0; i < n; ++i) {
            cnt += nums[i] <= mid;
        }

        if (cnt <= mid) {
            l = mid + 1;
        } else {
            r = mid - 1;
            ans = mid;
        }
    }
    return ans;
}

// 位运算

// Floy
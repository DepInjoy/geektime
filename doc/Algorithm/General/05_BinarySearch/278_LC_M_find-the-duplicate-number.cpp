/**
 * 287. 寻找重复数
 * https://leetcode.cn/problems/find-the-duplicate-number/description
*/

// 二分查找 时间复杂度O(NlogN), 空间复杂度为O(1)
int findDuplicate(vector<int>& nums) {
    int l = 0, r = nums.size() -1, ans = -1;
    while (l <= r) {
        int mid = l + r >> 1;
        int cnt = 0;
        for (int num : nums) {
            cnt += num <= mid;
        }
        if (cnt <= mid) {
            l = mid + 1;
        } else {
            ans = mid;
            r = mid - 1;
        }
    }
    return ans;
}

// 位运算

// Floy判环(龟兔赛跑)
int findDuplicate(vector<int>& nums) {
    int slow = 0, fast = 0;
    do {
        slow = nums[slow];
        fast = nums[nums[fast]];
        std::cout << slow << " " << fast << std::endl;
        if (slow == fast) {
            slow = 0;
            while (slow != fast) {
                slow = nums[slow];
                fast = nums[fast];
            }
            return slow;
        }
    } while (slow != fast);
    return -1;
}
/**
 * @file L239_MaxSlidingWindow.cpp
 * @author your name (you@domain.com)
 * @brief   给你一个整数数组 nums，有一个大小为 k 的滑动窗口从数组的最左侧移动到数组的最右侧。
 *          你只可以看到在滑动窗口内的 k 个数字。滑动窗口每次只向右移动一位。
 *          返回 滑动窗口中的最大值。
 *          链接：https://leetcode.cn/problems/sliding-window-maximum
 *          链接：https://leetcode.cn/problems/hua-dong-chuang-kou-de-zui-da-zhi-lcof/(剑指offer59)
 * 
 *          实现思想：
 *              1. 单调队列,时间复杂度O(N),空间复杂度为O(k)
 */


vector<int> maxSlidingWindow(vector<int>& nums, int k) {
    deque<int> q;
    vector<int> ans;
    for (int i = 0; i < nums.size(); i++) {
        if (!q.empty() && i - q.front() + 1 > k) q.pop_front();
        while(!q.empty() && nums[q.back()] <= nums[i]) q.pop_back();
        q.push_back(i);

        if (i >= k-1) ans.push_back(nums[q.front()]);
    }
    return ans;
}
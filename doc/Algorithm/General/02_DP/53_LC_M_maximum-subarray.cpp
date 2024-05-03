/**
 *  53. 最大子数组和
 *  https://leetcode.cn/problems/maximum-subarray/description/
 */

int maxSubArray(vector<int>& nums) {
    int pre = 0, cur = 0, ans = INT_MIN;
    for (int num : nums) {
        cur = std::max(pre + num, num);
        ans = std::max(ans, cur);
        pre = cur;
    }
    return ans;
}
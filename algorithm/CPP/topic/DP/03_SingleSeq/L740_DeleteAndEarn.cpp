/**
 * @brief   740. 删除并获得点数
 *          给你一个整数数组nums,你可以对它进行一些操作。每次操作中，选择任意一个nums[i]
 *          删除它并获得nums[i] 的点数。之后，你必须删除 所有等于nums[i]-1 和nums[i]+1的元素。
 *          开始你拥有 0 个点数。返回你能通过这些操作获得的最大点数。
 *          链接：https://leetcode.cn/problems/delete-and-earn
 *          
 *          实现思想：
 *              1. 动态规划,时间复杂度O(N), 空间复杂度为O(N+M)
 *                  N为数组的长度，M为数组的最大长度
 *              2. 动态规划+分组，时间复杂度为O(NlogN),空间复杂度为O(N)
 */

int deleteAndEarn(vector<int>& nums) {
    int maxVal = 0;
    for (auto num : nums) maxVal = max(maxVal, num);
    vector<int> dp(maxVal+1, 0);
    for (auto num : nums) {
        dp[num] += num;
    }

    // 入室抢劫
    int prepre = 0, pre = 0, cur = 0;
    for (int i = 0; i < dp.size(); i++) {
        cur = max(prepre+dp[i], pre);
        prepre = pre;
        pre = cur;
    }
    return cur;
}


int rob(const vector<int>& nums) {
    int prepre = 0, pre = 0, cur = 0;
    for (int i = 0; i < nums.size(); i++) {
        cur = max(prepre + nums[i], pre);
        prepre = pre;
        pre = cur;
    }
    return cur;
}

int deleteAndEarn(vector<int>& nums) {
    int ans = 0;
    sort(nums.begin(), nums.end());
    vector<int> dp{nums[0]};
    for (int i = 1; i < nums.size(); i++) {
        if (nums[i] == nums[i-1]) {
            dp.back() += nums[i];
        } else if (nums[i] == nums[i-1] + 1) {
            dp.push_back(nums[i]);
        } else {
            ans += rob(dp);
            dp = {nums[i]};
        }
    }
    ans += rob(dp);
    return ans;
}
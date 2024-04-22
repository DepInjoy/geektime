/**
 * 523. 连续的子数组和
 * https://leetcode.cn/problems/continuous-subarray-sum/description/
*/

// Hash表+前缀和
bool checkSubarraySum(vector<int>& nums, int k) {
    const int n = nums.size();
    std::unordered_map<int, int> mp;
    int remainer = 0;
    mp[0] = -1;
    for (int i = 0; i < n; ++i) {
        remainer = (remainer + nums[i]) % k;
        if (mp.count(remainer)) {
            int preIndex = mp[remainer];
            if (i - preIndex >= 2) return true;
        } else {
            mp[remainer] = i;
        }
    }
    return false;
}
/**
 * 15.三数之和
 *  https://leetcode.cn/problems/3sum/description/
*/

#include <vector>
using namespace std;

vector<vector<int>> threeSum(vector<int>& nums) {
    std::sort(nums.begin(), nums.end());
    std::vector<std::vector<int>> ans;
    const int n = nums.size();
    for (int i = 0; i < n; ++i) {
        int target = -nums[i];
        if (i - 1 >= 0 && nums[i] == nums[i-1]) continue;
        for (int j = i + 1, k = n - 1; j < n && j < k; ++j) {
            if (j - 1 > i && nums[j-1] == nums[j]) continue;
            while (j < k && nums[j] + nums[k] > target) --k;
            if (j < k && nums[j] + nums[k] == target) {
                ans.push_back({nums[i], nums[j], nums[k]});
            }
        }
    }
    return ans;
}
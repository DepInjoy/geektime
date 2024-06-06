/**
 * 15.三数之和
 *  https://leetcode.cn/problems/3sum/description/
*/

#include <vector>
using namespace std;

vector<vector<int>> threeSum(vector<int>& nums) {
    std::vector<std::vector<int>> ans;
    std::sort(nums.begin(), nums.end());
    const int n = nums.size();
    for (int i = 0; i < n; ++i) {
        if (i != 0 && nums[i] == nums[i-1]) continue;
        int target = -nums[i];
        for (int j = i + 1, k = n - 1; j < k; ++j) {
            if (j != i + 1 && nums[j] == nums[j-1]) continue;
            while (j < k && nums[j] + nums[k] > target) --k;
            if (j < k && nums[j] + nums[k] == target) {
                ans.push_back({nums[i], nums[j], nums[k]});
            }
        }
    }
    return ans;
}
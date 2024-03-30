/**
 * 16. 最接近的三数之和
 * https://leetcode.cn/problems/3sum-closest/description/
*/
#include <vector>
using namespace std;

int threeSumClosest(vector<int>& nums, int target) {
    const int n = nums.size();
    std::sort(nums.begin(), nums.end());
    int best = 1e7;
    for (int i = 0; i < n; ++i) {
        if (i > 0 && nums[i] == nums[i - 1]) continue;
        int j = i + 1, k = n - 1;
        while (j < k) {
            if (j > i + 1 && nums[j] == nums[j-1]) continue;

            int sum = nums[i] + nums[j] + nums[k];
            if (sum == target) return sum;

            if (std::abs(sum - target) < std::abs(best - target)) {
                best = sum;
            }
            if (sum > target) {
                int k0 = k - 1;
                while(j < k0 && nums[k0] == nums[k]) --k0;
                k = k0;
            } else {
                int j0 = j + 1;
                while(j0 < k && nums[j] == nums[j0]) ++j0;
                j = j0;
            }
        }
    }
    return best;
}
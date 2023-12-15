/**
 * 
*/
    vector<vector<int>> threeSum(vector<int>& nums) {
        std::sort(nums.begin(), nums.end());
        const int size = nums.size();
        vector<vector<int>> ans;
        for (int i = 0; i < size; ++i) {
            if (i > 0 && nums[i] == nums[i-1]) continue;

            int target = -nums[i];
            for (int j = i + 1, k = size - 1; j < size; ++j) {
                if (j > i + 1 && nums[j] == nums[j - 1]) continue;
                while (j < k && nums[j] + nums[k] > target) --k;

                if (j == k) break;

                if (nums[j] + nums[k] == target) {
                    ans.push_back({nums[i], nums[j], nums[k]});
                }
            }
        }
        return ans;
    }
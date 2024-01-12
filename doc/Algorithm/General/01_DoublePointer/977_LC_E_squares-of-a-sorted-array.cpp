/**
 * 977. 有序数组的平方
 * https://leetcode.cn/problems/squares-of-a-sorted-array/description/
*/

vector<int> sortedSquares(vector<int>& nums) {
    const int n = nums.size();
    std::vector<int> ans(n);
    int i = 0, j = n - 1, k = n - 1;
    while (i < n && i <= j) {
        int val1 = nums[i] * nums[i], val2 = nums[j] * nums[j];
        if (val1 > val2) {
            ans[k--] = val1, ++i;
        } else {
            ans[k--] = val2, --j;
        }
    }
    return ans;
}
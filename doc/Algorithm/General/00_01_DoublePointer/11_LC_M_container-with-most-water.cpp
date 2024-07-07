/**
 * 11. 盛最多水的容器
 * https://leetcode.cn/problems/container-with-most-water
 * 
 * 实现方式：
 *  1. 双指针
*/

int maxArea(vector<int>& height) {
    int ans = 0, l = 0, r = height.size() - 1;
    while (l < r) {
        if (height[l] < height[r]) {
            ans = std::max(ans, height[l] * (r - l));
            l++;
        } else {
            ans = std::max(ans, height[r] * (r - l));
            r--;
        }
    }
    return ans;
}


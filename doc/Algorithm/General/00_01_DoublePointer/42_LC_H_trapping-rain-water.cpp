/**
 * 42. 接雨水
 * https://leetcode.cn/problems/trapping-rain-water/
 * 
 * 实现方式：
 *  1. 动态规划
 *  2. 单调栈
 *  3. 双指针
*/

// 动态规划
int trap(vector<int>& height) {
    const int n = height.size();
    if (n == 0) return 0;

    std::vector<int> lMaxH(n);
    lMaxH[0] = height[0];
    for (int i = 1; i < n; ++i)
        lMaxH[i] = std::max(lMaxH[i-1], height[i]);

    std::vector<int> rMaxH(n);
    rMaxH[n-1] = height[n-1];
    for (int i = n - 2; i >= 0; i--)
        rMaxH[i] = std::max(rMaxH[i+1], height[i]);

    int ans = 0;
    for (int i = 0; i < n; ++i)
        ans += std::min(lMaxH[i], rMaxH[i]) - height[i];
    return ans;
}

// 单调栈

// 双指针

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
int trap(vector<int>& height) {
    std::stack<int> stk;
    int ans = 0;
    for (int i = 0; i < height.size(); ++i) {
        while (!stk.empty() && height[i] > height[stk.top()]) {
            int top = stk.top();
            stk.pop();
            if (stk.empty()) break;
            int left = stk.top();
            int width = i - left - 1;
            int waterH = std::min(height[i], height[left]) - height[top];
            ans += width * waterH;
        }
        stk.push(i);
    }
    return ans;
}

// 双指针
int trap(vector<int>& height) {
    int ans = 0;
    int leftMaxH = 0, rightMaxH = 0;
    int l = 0, r = height.size() - 1;
    while (l < r) {
        leftMaxH = std::max(leftMaxH, height[l]);
        rightMaxH = std::max(rightMaxH,  height[r]);
        if (height[l] < height[r]) {
            ans += leftMaxH - height[l++];
        } else {
            ans += rightMaxH - height[r--];
        }
    }
    return ans;
}
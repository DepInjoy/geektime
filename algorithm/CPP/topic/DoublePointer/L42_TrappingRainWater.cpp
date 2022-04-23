/**
 * @brief       给定 n 个非负整数表示每个宽度为 1 的柱子的高度图
 *              计算按此排列的柱子，下雨之后能接多少雨水。
 *              
 *              链接：https://leetcode-cn.com/problems/trapping-rain-water/
 */
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

int trap(vector<int>& height) {
    int ans = 0;
    int l = 0, r = height.size() - 1;
    int lMax = 0, rMax = 0;
    while(l < r) {
        lMax = max(lMax, height[l]);
        rMax = max(rMax, height[r]);
        // l到r间存在低洼地，计算左侧的盛水
        if (height[l] < height[r]) {
            ans += lMax - height[l];
            l++;
        } else { // 计算右侧的盛水
            ans += rMax - height[r];
            r--;
        }
    }
    return ans;
}

int main() {
    return 0;
}
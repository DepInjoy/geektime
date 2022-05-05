/**
 * @brief       给定 n 个非负整数表示每个宽度为 1 的柱子的高度图
 *              计算按此排列的柱子，下雨之后能接多少雨水。
 *              
 *              链接：https://leetcode-cn.com/problems/trapping-rain-water/
 * 
 *              实现思想：
 *                  1. 双指针，时间复杂度为O(N)，空间复杂度为O(1)
 *                  2. 动态规划，时间复杂度O(N)，空间复杂度O(N)
 *                  3. 单调栈
 */
#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;
// 双指针实现
int trap(vector<int>& height) {
    int N = height.size();
    if (N == 0) return 0;

    int ans = 0;
    int l = 0, r = N-1;
    int lmax = height[0], rmax = height[N-1];
    while(l <= r) {
        if (lmax < rmax) {
            lmax = max(lmax, height[l]);
            ans += lmax - height[l++];
        } else {
            rmax = max(rmax, height[r]);
            ans += rmax - height[r--];
        }
    }
    return ans;
}

// 动态规划实现
int trap(vector<int>& height) {
    int N = height.size();
    if (N == 0) return 0;

    vector<int> lmax = vector<int>(N);
    lmax[0] = height[0];
    for (int i = 1; i < N; i++) {
        lmax[i] = max(lmax[i-1], height[i]);
    }

    vector<int> rmax = vector<int>(N);
    rmax[N-1] = height[N-1];
    for (int i = N-2; i >= 0; i--) {
        rmax[i] = max(rmax[i+1], height[i]);
    }

    int ans = 0;
    for (int i = 0; i < N; i++) {
        ans += min(lmax[i], rmax[i]) - height[i];
        
    }
    return ans;
}
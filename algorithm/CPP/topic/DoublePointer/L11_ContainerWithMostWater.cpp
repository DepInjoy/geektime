/**
 * @brief   给定一个长度为n的整数数组height.有n条垂线，第i条线的两个端点是(i, 0)和(i, height[i])
 *          找出其中的两条线，使得它们与 x 轴共同构成的容器可以容纳最多的水。
 *          返回容器可以储存的最大水量。
 * 
 *          链接：https://leetcode-cn.com/problems/container-with-most-water
 * 
 * 实现思想：
 *      采用双指针，l从头开始，自左向右，r从尾开始，自右向左
 *      if (height[l] >= height[r])
 *          r--;
 *      else
 *          l++
 */

#include <iostream>
#include <vector>

using namespace std;

int maxArea(vector<int>& height) {
    int ans = 0;
    int l = 0, r = height.size()-1;
    while(l < r) {
        ans = max(ans, min(height[l], height[r])*(r-l));
        (height[l] >= height[r]) ? r-- : l++;
    }
    return ans;
}

// 实现更加简洁
int maxArea(vector<int>& height) {
    int ans = 0;
    int l = 0, r = height.size()-1;
    while(l < r) {
        ans = max(ans, min(height[l], height[r])*(r-l));
        if (height[l] >= height[r]) {
            r--;
        } else {
            l++;
        }
    }
    return ans;
}

int main() {
    return 0;
}
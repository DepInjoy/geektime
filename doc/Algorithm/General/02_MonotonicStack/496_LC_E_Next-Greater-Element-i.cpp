/**
 *  496. 下一个更大元素 I
 *  https://leetcode.cn/problems/next-greater-element-i/description/
*/
#include<vector>
using namespace std;

vector<int> nextGreaterElement(vector<int>& nums1, vector<int>& nums2) {
    const int m = nums1.size(), n = nums2.size();
    std::unordered_map<int, int> ump(n);
    std::stack<int> stk;
    for (int i = n - 1; i >= 0; --i) {
        while(!stk.empty() && nums2[i] >= stk.top()) {
            stk.pop();
        }
        ump[nums2[i]] = stk.empty() ? -1 : stk.top();
        stk.push(nums2[i]);
    }

    std::vector<int> ans(m);
    for (int i = 0; i < m; ++i) {
        ans[i] = ump[nums1[i]];
    }
    return ans;
}
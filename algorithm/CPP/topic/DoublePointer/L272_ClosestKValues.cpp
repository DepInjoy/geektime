/**
 * @brief   给定二叉搜索树的根 root 、一个目标值 target 和一个整数 k
 *          返回BST中最接近目标的 k 个值。你可以按 任意顺序 返回答案。
 *          题目保证该二叉搜索树中只会存在一种 k 个值集合最接近 target
 * 
 *          链接：https://leetcode-cn.com/problems/closest-binary-search-tree-value-ii
 * 
 */
#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;

vector<int> closestKValues(TreeNode* root, double target, int k) {
    if (root == null) return vector<int>();

    vector<int> ans;
    std::vector<int> arr;
    inorderPost(root, arr);
    int sum = 0;
    int index = 0;
    for (int i = 0; i < arr.size(); i++) {
        if (ans.size() < k) {
            index++;
            sum += arr[i];
            ans.push_back(arr[i]);
        } else {
            if (abs(sum - ans[index%(k-1)] + arr[i] - target) < abs(sum)) {
                index++;
                sum += -ans[index%(k-1)] + arr[i]
            }
        }
    }
    return ans;
}

void inorderPost(TreeNode* root, std::vector<int> arr) {
    if (root == null) return;

    if (root.left != null) inorderPost(root.left, arr);
    arr.push_back(root.value);
    if (root.right != null) inorderPost(root.right, arr);
}

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

struct TreeNode {
    int val;
    TreeNode *left;
    TreeNode *right;
    TreeNode() : val(0), left(nullptr), right(nullptr) {}
    TreeNode(int x) : val(x), left(nullptr), right(nullptr) {}
    TreeNode(int x, TreeNode *left, TreeNode *right) :
        val(x), left(left), right(right) {}
};

vector<int> closestKValues(TreeNode* root, double target, int k) {
    if (root == NULL || k <= 0) return vector<int>();

    vector<int> ans;
    std::vector<int> arr;
    int index = 0; // 最接近target的位置索引
    // 对搜索二叉树中序遍历得到有序数组
    inorderPost(root, arr);
    // 双指针算法给index复制
    for (int i = 0, j = arr.size()-1; i < arr.size(); i++) {
        while(i < j && abs(arr[i] - target) < abs(arr[j] - target)) {
            j--;
        }
        index = j;
    }

    ans.push_back(arr[index]);
    int l = index - 1, r = index + 1;
    // 以index为中心，分别向左、右推进进行数据填充
    while (ans.size() < k) {
        if (l >= 0 && r < arr.size()) {
            if (abs(arr[l] - target) < abs(arr[r] - target)) {
                ans.push_back(arr[l--]);
            } else {
                ans.push_back(arr[r++]);
            }
        } else if (l < 0) {
            ans.push_back(arr[r++]);
        } else if (r >= arr.size()) {
            ans.push_back(arr[l--]);
        }
    }
    return ans;
}

void inorderPost(TreeNode* root, std::vector<int>& arr) {
    if (root == NULL) return;

    if (root->left != NULL) inorderPost(root->left, arr);
    arr.push_back(root->val);
    if (root->right != NULL) inorderPost(root->right, arr);
}

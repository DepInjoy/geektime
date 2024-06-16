/**
 * 104. 二叉树的最大深度
 * https://leetcode.cn/problems/maximum-depth-of-binary-tree
 * 
 * 实现方式：
 *  1. 深度优先搜索
 *  2. 广度优先搜索
 */

// 深度优先搜索,时间复杂度O(N), 空间复杂度O(H)
int maxDepth(TreeNode* root) {
    if (!root) return 0;
    return std::max(maxDepth(root->left), maxDepth(root->right)) + 1;
}

// 广度优先搜索, 时间复杂度O(N), 空间复杂度最坏可达O(N)
int maxDepth(TreeNode* root) {
    if (!root) return 0;

    int ans = 0;
    std::queue<TreeNode*> q;
    q.push(root);
    while (!q.empty()) {
        int curWidth = q.size();
        for (int i = 0; i < curWidth; ++i) {
            TreeNode* cur = q.front();
            q.pop();
            if (cur->left) q.push(cur->left);
            if (cur->right) q.push(cur->right);
        }
        ++ans;
    }
    return ans;
}


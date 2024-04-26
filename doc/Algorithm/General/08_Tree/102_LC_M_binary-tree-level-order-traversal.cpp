/**
 * 102. 二叉树的层序遍历s
 * https://leetcode.cn/problems/binary-tree-level-order-traversal/
*/

vector<vector<int>> levelOrder(TreeNode* root) {
    std::vector<std::vector<int>> ans;
    if (!root) return ans;

    std::queue<TreeNode*> q;
    q.push(root);
    while (!q.empty()) {
        int curLevelSize = q.size();
        ans.push_back(std::vector<int>{});
        for (int i = 0; i < curLevelSize; ++i) {
            auto cur = q.front();
            q.pop();
            ans.back().push_back(cur->val);
            if (cur->left) q.push(cur->left);
            if (cur->right) q.push(cur->right);
        }
    }
    return ans;
}
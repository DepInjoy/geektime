/**
 * 105. 从前序与中序遍历序列构造二叉树
 * https://leetcode.cn/problems/construct-binary-tree-from-preorder-and-inorder-traversal
*/

// 递归，迭代,时间复杂度O(N), 空间复杂度O(n+h) ≈ O(n)
TreeNode* helper(int pre_left, int pre_right, int in_left, int in_right,
        const vector<int>& preorder,const vector<int>& inorder,
        std::unordered_map<int, int>& index_map) {
    if (pre_left > pre_right) return nullptr;

    int root_val = preorder[pre_left];
    TreeNode* root = new TreeNode(root_val);

    int in_idx = index_map[root_val];
    int left_subtree_size = in_idx - in_left;
    root->left = helper(pre_left + 1, pre_left + left_subtree_size,
            in_left, in_idx - 1,
            preorder, inorder, index_map);
    root->right = helper(pre_left + left_subtree_size + 1, pre_right,
            in_idx + 1, in_right,
            preorder, inorder, index_map);
    return root;
}

TreeNode* buildTree(vector<int>& preorder, vector<int>& inorder) {
    std::unordered_map<int, int> index_map;
    for (int i = 0; i < inorder.size(); ++i) index_map[inorder[i]] = i;
    return helper(0, preorder.size() - 1, 0, inorder.size() - 1, preorder, inorder, index_map);
}

// 迭代,时间复杂度O(N), 空间复杂度O(h)
TreeNode* buildTree(vector<int>& preorder, vector<int>& inorder) {
    if (preorder.size() == 0) return nullptr;

    std::stack<TreeNode*> stk;
    int inOrderIndex = 0;
    TreeNode* root = new TreeNode(preorder[0]);
    stk.push(root);
    for (int i = 1; i < preorder.size(); ++i) {
        TreeNode* node = stk.top();
        if (node->val != inorder[inOrderIndex]) {
            node->left = new TreeNode(preorder[i]);
            stk.push(node->left);
        } else {
            while (!stk.empty() && stk.top()->val == inorder[inOrderIndex]) {
                node = stk.top();
                stk.pop();
                ++inOrderIndex;
            }
            node->right = new TreeNode(preorder[i]);
            stk.push(node->right);
        }
    }
    return root;
}
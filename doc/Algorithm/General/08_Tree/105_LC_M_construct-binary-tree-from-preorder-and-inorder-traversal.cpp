/**
 * 105. 从前序与中序遍历序列构造二叉树
 * https://leetcode.cn/problems/construct-binary-tree-from-preorder-and-inorder-traversal
*/

// 递归
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

// 
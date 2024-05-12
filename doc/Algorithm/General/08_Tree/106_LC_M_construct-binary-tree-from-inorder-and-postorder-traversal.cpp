/**
 * 106. 从中序与后序遍历序列构造二叉树
 * https://leetcode.cn/problems/construct-binary-tree-from-inorder-and-postorder-traversal/
 * 
 * 实现方式:
 *  1. 递归实现
 *  2. 
*/

// 递归
TreeNode* helper(int in_left, int in_right, const std::vector<int>& inorder,
        const std::vector<int>& postorder, int& post_index,
        std::unordered_map<int, int>& index_map) {
    if (in_left > in_right) return nullptr;

    int root_val = postorder[post_index];
    TreeNode* root = new TreeNode(root_val);
    int in_index = index_map[root_val];
    post_index--;

    root->right = helper(in_index + 1, in_right, inorder, postorder, post_index, index_map);
    root->left = helper(in_left, in_index - 1, inorder, postorder, post_index, index_map);
    return root;
}

TreeNode* buildTree(vector<int>& inorder, vector<int>& postorder) {
    int post_index = postorder.size() - 1;
    std::unordered_map<int, int> index_map;
    for (int i = 0; i < inorder.size(); ++i) index_map[inorder[i]] = i;
    return helper(0, inorder.size() - 1, inorder, postorder, post_index, index_map);
}
    
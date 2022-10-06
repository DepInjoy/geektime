/**
 * @brief   二叉树遍历
 *          94. 二叉树的中序遍历
 *              https://leetcode.cn/problems/binary-tree-inorder-traversal/
 *          144. 二叉树的前序遍历
 *              https://leetcode.cn/problems/binary-tree-preorder-traversal/
 *          145. 二叉树的后序遍历
 *              https://leetcode.cn/problems/binary-tree-postorder-traversal/
 * 
 */

// 前序遍历
void preorderTraversal(TreeNode* root, std::vector<int>& res) {
    if (!root) return;

    res.push_back(root->val);
    if (root->left) preorderTraversal(root->left, res);
    if (root->right) preorderTraversal(root->right, res);
}

vector<int> preorderTraversal(TreeNode* root) {
    std::vector<int> res;
    preorderTraversal(root, res);
    return res; 
}

// 中序遍历
void inorderTraversal(TreeNode* root, std::vector<int>& res) {
    if (!root) return;

    if (root->left) inorderTraversal(root->left, res);
    res.push_back(root->val);
    if (root->right) inorderTraversal(root->right, res);
}

vector<int> inorderTraversal(TreeNode* root) {
    if (!root) return {};

    std::vector<int> res;
    inorderTraversal(root, res);
    return res; 
}

// 后续遍历
void postorderTraversal(TreeNode* root, std::vector<int>& res) {
    if (!root) return;

    if (root->left) postorderTraversal(root->left, res);
    if (root->right) postorderTraversal(root->right, res);
    res.push_back(root->val);
}

vector<int> postorderTraversal(TreeNode* root) {
    std::vector<int> res;
    postorderTraversal(root, res);
    return res; 
}
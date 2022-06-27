/**
 * @brief   面试题 04.05. 合法二叉搜索树
 *          实现一个函数，检查一棵二叉树是否为二叉搜索树。
 *          链接：https://leetcode.cn/problems/legal-binary-search-tree-lcci/
 */

bool isValidBST(TreeNode* root) {
    return helper(root, LONG_MIN, LONG_MAX);
}

bool helper(TreeNode* root, long  lower, long upper) {
    if (!root) return true;
    if (root->val <= lower || root->val >= upper) return false;
    return helper(root->left, lower, root->val) &&
            helper(root->right, root->val, upper);
}
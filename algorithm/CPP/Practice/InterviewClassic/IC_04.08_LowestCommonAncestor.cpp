/**
 * @brief   面试题 04.08. 首个共同祖先
 *          设计并实现一个算法，找出二叉树中某两个节点的第一个共同祖先
 *          不得将其他的节点存储在另外的数据结构中。注意：这不一定是二叉搜索树。
 *          链接：https://leetcode.cn/problems/first-common-ancestor-lcci/
 */

TreeNode* ans{nullptr};
bool dfs(TreeNode* root, TreeNode* p, TreeNode* q) {
    if (!root) return false;
    bool lson = dfs(root->left, p, q);
    bool rson = dfs(root->right, p, q);
    if ((lson && rson) || ((root->val == p->val ||
        root->val == q->val) && (lson || rson))) ans = root;
    return lson || rson || p->val == root->val || q->val == root->val;
}

TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
    dfs(root, p, q);
    return ans;
}
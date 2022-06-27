/**
 * @brief   面试题 04.04. 检查平衡性
 *          实现一个函数，检查二叉树是否平衡。在这个问题中，
 *          平衡树的定义如下：任意一个节点，其两棵子树的高度差不超过 1。
 *          链接：https://leetcode.cn/problems/check-balance-lcci/
 */
bool isBalanced(TreeNode* root) {
    return TreeHeight(root) >= 0;
}

int TreeHeight(TreeNode* root) {
    if (!root) return 0;

    int lh = TreeHeight(root->left);
    int rh = TreeHeight(root->right);
    if (lh == -1 || rh == -1 || abs(lh - rh) > 1) {
        return -1;
    } else {
        return max(lh, rh) + 1;
    }
}

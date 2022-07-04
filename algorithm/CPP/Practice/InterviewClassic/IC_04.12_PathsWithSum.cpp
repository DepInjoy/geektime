/**
 * @brief   面试题 04.12. 求和路径
 *          给定一棵二叉树，其中每个节点都含有一个整数数值(该值或正或负)。
 *          设计一个算法，打印节点数值总和等于某个给定值的所有路径的数量。
 *          注意，路径不一定非得从二叉树的根节点或叶节点开始或结束，
 *          但是其方向必须向下(只能从父节点指向子节点方向)。
 *          链接：https://leetcode.cn/problems/paths-with-sum-lcci/
 */

// 时间复杂度O(N^2), 空间复杂度为O(N)
int rootSum(TreeNode* root, const int sum) {
    if (!root) return 0;
    int ans = 0;
    if (root->val == sum) ans++;
    ans += rootSum(root->left, sum - root->val);
    ans += rootSum(root->right, sum - root->val);
    return ans;
}

int pathSum(TreeNode* root, int sum) {
    if (!root) return 0;
    
    int ans = 0;
    ans += rootSum(root, sum);
    ans += pathSum(root->left, sum);
    ans += pathSum(root->right, sum);
    return ans;
}
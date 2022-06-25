/**
 * @brief   面试题 04.02. 最小高度树
 *          给定一个有序整数数组，元素各不相同且按升序排列
 *          编写一个算法，创建一棵高度最小的二叉搜索树。
 *          链接：https://leetcode.cn/problems/minimum-height-tree-lcci/
 */

TreeNode* sortedArrayToBST(vector<int>& nums) {
    return sortedArrayToBST(nums, 0, nums.size()-1);
}

TreeNode* sortedArrayToBST(vector<int>& nums, const int left, const int right) {
    if (right < left) return NULL;

    int mid = (left + right + 1) /2;
    TreeNode* root = new TreeNode(nums[mid]);
    root->left = sortedArrayToBST(nums, left, mid-1);
    root->right = sortedArrayToBST(nums, mid+1, right);
    return root;
}
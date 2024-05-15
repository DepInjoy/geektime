/**
 * 94. 二叉树的中序遍历
 * https://leetcode.cn/problems/binary-tree-inorder-traversal/description/
 * 
 * 相关实现:
 *    1. 递归实现
 *    2. 非递归实现
 *    3.  Morris中序遍历(TODo)
*/

// 1. 递归实现
void inorderTraversal(TreeNode* root, std::vector<int>& data) {
    if (!root) return;

    inorderTraversal(root->left, data);
    data.push_back(root->val);
    inorderTraversal(root->right, data);
}

vector<int> inorderTraversal(TreeNode* root) {
    std::vector<int> data;
    inorderTraversal(root, data);
    return data;
}

// 2. 非递归实现
vector<int> inorderTraversal(TreeNode* root) {
    if (!root) return {};

    std::vector<int> data;
    std::stack<TreeNode*> stk;
    while (root || !stk.empty()) {
        while (root) {
            stk.push(root);
            root = root->left;
        }

        root = stk.top();
        stk.pop();
        data.push_back(root->val);
        root = root->right;
    }
    return data;
}

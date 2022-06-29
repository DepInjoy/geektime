/**
 * @brief   面试题 04.06. 后继者
 *          设计一个算法，找出二叉搜索树中指定节点的“下一个”节点（也即中序后继）。
 *          如果指定节点没有对应的“下一个”节点，则返回null。
 *          链接：https://leetcode.cn/problems/successor-lcci/
 */

// 中序遍历，找到匹配的便返回
TreeNode* inorderSuccessor(TreeNode* root, TreeNode* p) {
    stack<TreeNode*> stk;
    TreeNode* pre = nullptr;
    TreeNode* cur = root;
    while(!stk.empty() || cur) {
        if (cur) {
            stk.push(cur);
            cur = cur->left;
        } else {
            cur = stk.top();
            stk.pop();
            if (pre == p) return cur;
            pre = cur;
            cur = cur->right;
        }
    }
    return nullptr;
}

TreeNode* inorderSuccessor(TreeNode* root, TreeNode* p) {
    TreeNode* successor = nullptr;
    if (p->right) {
        successor = p->right;
        while (successor->left != nullptr) {
            successor = successor->left;
        }
        return successor;
    }

    TreeNode* cur = root;
    while (cur) {
        if (cur->val > p->val) {
            successor = cur;
            cur = cur->left;
        } else {
            cur = cur->right;
        }
    }
    return successor;
}
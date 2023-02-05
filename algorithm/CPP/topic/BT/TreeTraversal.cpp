/**
 * @brief       二叉树遍历实现
 *              1. 深度优先遍历,可以是前序遍历, 中序遍历和后序遍历
 *              2. 宽度优先遍历
 */
#include <iostream>
#include <stack>

struct TreeNode {
    int         val;
    TreeNode*   left;
    TreeNode*   right;
};

// 代表对当前节点的处理操作,根据实际使用场景存在不同使用
void ProcessNode(TreeNode* node) {
    std::cout << "val = " << node->val << std::endl;
}

/**
 * @brief   二叉树深度优先前序遍历的递归遍历和非递归遍历
 *          [144.二叉树的前序遍历](https://leetcode.cn/problems/binary-tree-preorder-traversal/description/)
 */
// 二叉树深度优先前序遍历的非递归遍历
void PreorderTraversal(TreeNode* node) {
    if (!node) {
        return;
    }

    ProcessNode(node);
    PreorderTraversal(node->left);
    PreorderTraversal(node->right);
}

// 二叉树深度优先前序遍历的非递归遍历
void PreorderTraversalNonRecursive(TreeNode* node) {
    if (!node) return;

    std::stack<TreeNode*>* localStack = new std::stack<TreeNode*>();
    TreeNode* cur = node;
    while (cur || !localStack->empty()) {
        while (cur) {
            ProcessNode(cur);
            localStack->push(cur);
            cur = cur->left;
        }

        cur = localStack->top();
        localStack->pop();
        cur = cur->right;
    }
    delete localStack;
}

/**
 * @brief   二叉树深度优先中序遍历的递归遍历和非递归遍历
 *          [LeetCode-94.二叉树的中序遍历](https://leetcode.cn/problems/binary-tree-inorder-traversal/description/)
 */
// 二叉树深度优先中序遍历的递归遍历
void InorderTraversal(TreeNode* node) {
    if (node) {
        return;
    }

    InorderTraversal(node->left);
    ProcessNode(node);
    InorderTraversal(node->right);
}

// 二叉树深度优先中序遍历的非递归遍历
void InorderTraversalNonRecursive(TreeNode* node) {
    if (!node) return;

    std::stack<TreeNode*>* localStack = new std::stack<TreeNode*>();
    TreeNode* cur = node;
    while (cur || !localStack->empty()) {
        while (cur) {
            localStack->push(cur);
            cur = cur->left;
        }

        cur = localStack->top();
        localStack->pop();
        ProcessNode(cur);
        cur = cur->right;
    }
    delete localStack;
}

/**
 * @brief   二叉树深度优先后序遍历的递归遍历
 */
void PostorderTraversal(TreeNode* node) {
    if (node) {
        PostorderTraversal(node->left);
        PostorderTraversal(node->right);
        ProcessNode(node);
    }
}

/**
 * @brief   二叉树深度优先后序遍历的非递归遍历
 */
void PostorderTraversalNonRecursive(TreeNode* node) {
    if (!node) return;

    std::stack<TreeNode*>* localStack = new std::stack<TreeNode*>();
    TreeNode* cur = node;
    TreeNode* pre = nullptr;
    while (cur || !localStack->empty()) {
        while (cur) {
            localStack->push(cur);
            cur = cur->left;
        }

        cur = localStack->top();
        if (cur->right && cur->right != pre) { // 右子树还没处理
            cur = cur->right;
        } else { // 右子树处理完成, 处理当前节点
            localStack->pop();
            ProcessNode(cur);
            pre = cur;
            cur = nullptr;
        }
    }
    delete localStack;
}
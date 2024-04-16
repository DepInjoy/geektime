/**
 * 94. 二叉树的中序遍历
 * https://leetcode.cn/problems/binary-tree-inorder-traversal/description/
 * 
 * 144. 二叉树的前序遍历
 * https://leetcode.cn/problems/binary-tree-preorder-traversal/description/
 * 
 * 145. 二叉树的后序遍历
 * https://leetcode.cn/problems/binary-tree-postorder-traversal/description/
 * 
 * 102. 二叉树的层序遍历
 * https://leetcode.cn/problems/binary-tree-level-order-traversal/description/
*/

// 1.1 二叉树中序遍历(递归实现)
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

// 1.2 二叉树中序遍历(非递归实现)
vector<int> inorderTraversal(TreeNode* root) {
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


// 2.1 前序遍历(递归实现)
void preorderTraversal(TreeNode* root, std::vector<int>& data) {
    if (!root) return;

    data.push_back(root->val);
    preorderTraversal(root->left, data);
    preorderTraversal(root->right, data);
}

vector<int> preorderTraversal(TreeNode* root) {
    std::vector<int> data;
    preorderTraversal(root, data);
    return data;
}

// 2.2 前序遍历(非递归实现)
vector<int> preorderTraversal(TreeNode* root) {
    std::vector<int> data;
    std::stack<TreeNode*> stk;
    while (root || !stk.empty()) {
        while (root) {
            data.push_back(root->val);
            stk.push(root);
            root = root->left;
        }
        root = stk.top();
        stk.pop();
        root = root->right;
    }
    return data;
}

// 3.1 后序遍历(递归实现)
void postorderTraversal(TreeNode* root, std::vector<int>& data) {
    if (!root) return;

    postorderTraversal(root->left, data);
    postorderTraversal(root->right, data);
    data.push_back(root->val);
}

vector<int> postorderTraversal(TreeNode* root) {
    std::vector<int> data;
    postorderTraversal(root, data);
    return data;
}

// 3.2 后序遍历(非递归实现)
vector<int> postorderTraversal(TreeNode* root) {
    std::vector<int> data;
    std::stack<TreeNode*> stk;
    TreeNode *prev = nullptr;
    while (root || !stk.empty()) {
        while (root != nullptr) {
            stk.push(root);
            root = root->left;
        }

        root = stk.top();
        stk.pop();
        if (root->right == nullptr || (root->right == prev)) {
            data.push_back(root->val);
            prev = root;
            root = nullptr;
        } else {
            stk.push(root);
            root = root->right;
        }
    }
    return data;
}

// 层次遍历
vector<vector<int>> levelOrder(TreeNode* root) {
    std::vector<std::vector<int>> data;
    if (!root) return data;

    std::queue<TreeNode*> q;
    q.push(root);
    while (!q.empty()) {
        int curLevelSize = q.size();
        data.push_back(std::vector<int>{});
        for (int i = 1; i <= curLevelSize; ++i) {
            TreeNode* node = q.front();
            q.pop();
            data.back().push_back(node->val);
            if (node->left) q.push(node->left);
            if (node->right) q.push(node->right);
        }
    }
    return data;
}
/**
 * 236. 二叉树的最近公共祖先
 * https://leetcode.cn/problems/lowest-common-ancestor-of-a-binary-tree/description/
 * 
 * 实现方式：
 *  1. 递归
 *  2. Hash表存储父节点
*/

// 递归
bool dfs(TreeNode* root, TreeNode* p, TreeNode* q, TreeNode** commonAncestor) {
    if (!root) return false;
    bool lson = dfs(root->left, p, q, commonAncestor);
    bool rson = dfs(root->right, p, q, commonAncestor);
    if ((lson && rson) || (root->val == p->val || root->val == q->val) && (lson || rson)) {
        *commonAncestor = root;
    }

    return lson || rson || (root->val == p->val || root->val == q->val);
}

TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
    TreeNode* ans;
    dfs(root, p, q, &ans);
    return ans;
}

// Hash表存储父节点
void dfs(TreeNode* root, std::unordered_map<int, TreeNode*>& fa,
        std::unordered_map<int, bool>& visited) {
    if (!root) return;
    if (root->left) {
        fa[root->left->val] = root;
        dfs(root->left, fa, visited);
    }

    if (root->right) {
        fa[root->right->val] = root;
        dfs(root->right, fa, visited);
    }
}

TreeNode* lowestCommonAncestor(TreeNode* root, TreeNode* p, TreeNode* q) {
    std::unordered_map<int, TreeNode*> fa;
    std::unordered_map<int, bool> visited;
    dfs(root, fa, visited);

    while (p) {
        visited[p->val] = true;
        p = fa[p->val];
    }

    while (q) {
        if (visited[q->val]) return q;
        q = fa[q->val];
    }
    return nullptr;
}
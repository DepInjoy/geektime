/**
 * @brief   面试题 04.10. 检查子树
 *          检查子树。你有两棵非常大的二叉树：T1，有几万个节点；T2，有几万个节点。
 *          设计一个算法，判断 T2 是否为 T1 的子树。
 *          如果 T1 有这么一个节点 n，其子树与 T2 一模一样，则 T2 为 T1 的子树，
 *          也就是说，从节点 n 处把树砍断，得到的树与 T2 完全相同。
 *          链接：https://leetcode.cn/problems/check-subtree-lcci/
 */


bool checkSubTree(TreeNode* t1, TreeNode* t2) {
    if (!t2) return true;
    if (!t1) return false;

    std::string str1, str2;
    getOrderedString(t1, str1);
    getOrderedString(t2, str2);
    return str1.find(str2) != string::npos;
}

void getOrderedString(TreeNode* t, std::string& str) {
    if (!t) {
        str.append("null");
        return;
    }
    str.append(std::to_string(t->val));
    getOrderedString(t->left, str);
    getOrderedString(t->right, str);
}
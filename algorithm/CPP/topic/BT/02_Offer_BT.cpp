#include "TreeNode.h"
#include <string>

/**
 * 47：二叉树剪枝
 *      题目：一棵二叉树的所有节点的值要么是0要么是1，
 *          请剪除该二叉树中所有节点的值全都是0的子树。
 */

TreeNode* pruneTree(TreeNode* root) {
    if (!root) return root;

    root->left = pruneTree(root->left);
    root->right = pruneTree(root->right);
    if (!root->left && !root->right && root->val == 0) {
        reurn nullptr;
    }
    return root;  
}

/**
 * 48：序列化和反序列化二叉树
 *      题目：请设计一个算法将二叉树序列化成一个字符串，
 *          并能将该字符串反序列化出原来二叉树的算法。
 */

std::string serialize(TreeNode* root) {
    if (!root) return "#";

    std::string leftStr = serialize(root->left);
    std::string rightStr = serialize(root->right);
    std::string ans = std::to_string(root->val);
    ans.append(",").append(leftStr).append(",").append(rightStr)
    return ans;    
}

TreeNode* deserialize(std::string& data) {

}

/**
 * 49：从根节点到叶节点的路径数字之和
 *      题目：在一棵二叉树中所有节点都在0～9的范围之内，
 *      从根节点到叶节点的路径表示一个数字。
 */

 int sumNumbers(TreeNode* root) {

 }

 /**
  * 50：向下的路径节点值之和
  *     题目：给定一棵二叉树和一个值sum，求二叉树中节点值之和等于sum的路径的数目。
  *     路径的定义为二叉树中顺着指向子节点的指针向下移动所经过的节点，但不一定从根
  *     节点开始，也不一定到叶节点结束。
  */
 int pathSum(TreeNode* root, int sum) {

 }

 /**
  * 51：节点值之和最大的路径
  *     题目：在二叉树中将路径定义为顺着节点之间的连接从任意一个节点开始到达任意
  *     一个节点所经过的所有节点。路径中至少包含一个节点，不一定经过二叉树的根节点，
  *     也不一定经过叶节点。给定非空的一棵二叉树，请求出二叉树所有路径上节点值之和
  *     的最大值。
  */

 int maxPathSum(TreeNode root) {

 }
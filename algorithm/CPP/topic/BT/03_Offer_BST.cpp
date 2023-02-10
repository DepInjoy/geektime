/**
 * @brief   二叉搜索树-剑指offer专项
 * 
 */
# include "TreeNode.h"

/**
 * 二叉搜索树查找节点，时间复杂度O(h)
 */
TreeNode* searchBST(TreeNode* root, int val) {

}

/**
 * 52. 展开二叉搜索树
 *      给定一棵二叉搜索树，请调整节点的指针使每个节点都没有左子节点。
 *      调整之后的树看起来像一个链表，但仍然是二叉搜索树。
 */

TreeNode* increasingBST(TreeNode* root) {

}

/**
 * 53. 二叉搜索树的下一个节点
 *      题目：给定一棵二叉搜索树和它的一个节点p
 *      请找出按中序遍历的顺序该节点p的下一个节点。
 *      假设二叉搜索树中节点的值都是唯一的
 */
 TreeNode* inorderSuccessor(TreeNode* root, TreeNode* p) {

 }

 /**
  * 54：所有大于或等于节点的值之和
  *     题目：给定一棵二叉搜索树，请将它的每个节点的值替换成树中
  *     大于或等于该节点值的所有节点值之和。
  *     假设二叉搜索树中节点的值唯一
  */
 TreeNode* convertBST(TreeNode* root) {

 }

 /**
  * 55：二叉搜索树迭代器
  *     题目：请实现二叉搜索树的迭代器BSTIterator，它主要有如下3个函数。
  *         构造函数：输入二叉搜索树的根节点初始化该迭代器。
  *         函数next：返回二叉搜索树中下一个最小的节点的值。
  *         函数hasNext：返回二叉搜索树是否还有下一个节点。
  */
class BSTIterator {
public:
    BSTIterator(TreeNode* root) {}
    int next();
    bool hasNext();
};

/**
 * 56：二叉搜索树中两个节点的值之和
 *      题目：给定一棵二叉搜索树和一个值k,
 *      请判断该二叉搜索树中是否存在值之和等于k的两个节点。
 *      假设二叉搜索树中节点的值均唯一。
 */

bool findTarget(TreeNode* root, int k) {

}
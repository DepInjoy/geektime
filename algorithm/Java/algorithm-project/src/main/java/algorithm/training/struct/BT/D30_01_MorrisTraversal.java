package algorithm.training.struct.BT;

/**
 * @brief:          Morris遍历二叉树
 *                  时间复杂度O(N)，额外空间复杂度O(1)
 *                  实现方式：
 *                      假设来到当前节点cur，开始时cur来到头节点位置
 *                          1. 如果cur没有左孩子，cur向右移动(cur = cur.right)
 *                          2. 如果cur有左孩子，找到左子树上最右的节点mostRight：
 * 	                            a.如果mostRight的右指针指向空，让其指向cur，然后cur向左移动(cur = cur.left)
 * 	                            b.如果mostRight的右指针指向cur，让其指向null, 然后cur向右移动(cur = cur.right)
 *                          3. cur为空时遍历停止
 *                  核心思想: 通过利用原树中大量空闲指针的方式，达到节省空间的目的,它建立一种机制:
 *                      对于没有左子树的节点只到达一次, 对于有左子树的节点会到达两次, morris遍历时间复杂度依然是O(N)
 * */
public class D30_01_MorrisTraversal {
}

/**
 * 141. 环形链表
 * https://leetcode.cn/problems/linked-list-cycle/description/
 * 
 * 实现方式1 [set判重]:
 *     将链表中的元素一一插入set,利用其中数据元素不可重复的特性
 *     当插入失败则可以认为数据中有环，否则无环
 *     时间复杂度O(N), 空间复杂度为O(N)
 *
 * 实现方式2 [快慢指针]:
 *     通过快慢指针实现，快指针每次走两步，慢指针每次走一步
 *     转一圈两者相遇则说明存在环，否则不存在环
 *     时间复杂度O(N), 空间复杂度为O(1)。
*/

// set判重
bool hasCycle(ListNode *head) {
    std::unordered_set<ListNode*> nodes;
    while (head) {
        if (nodes.count(head)) return true;
        nodes.insert(head);
        head = head->next;
    }
    return false;
}

// 快慢指针
bool hasCycle(ListNode *head) {
    ListNode* slow = head, *fast = head;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) {
            return true;
        }
    }
    return false;
}
/**
 * 141. 环形链表
 * https://leetcode.cn/problems/linked-list-cycle/description/
*/

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
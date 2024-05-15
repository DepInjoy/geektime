/**
 * 142. 环形链表 II
 * https://leetcode.cn/problems/linked-list-cycle-ii/description/
*/

ListNode *detectCycle(ListNode *head) {
    ListNode* slow = head, *fast = head;
    bool hasCycle = false;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) {
            slow = head;
            while (slow != fast) {
                slow = slow->next;
                fast = fast->next;
            }
            return slow;
        }
    }
    return nullptr;
}
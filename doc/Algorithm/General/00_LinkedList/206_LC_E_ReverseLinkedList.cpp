/**
 * 206. 反转链表
 *  https://leetcode.cn/problems/reverse-linked-list/description/
*/

ListNode* reverseList(ListNode* head) {
    ListNode* pre = nullptr, *cur = head;
    while (cur) {
        ListNode* next = cur->next;
        cur->next = pre;
        pre = cur;
        cur = next;
    }
    return pre;
}
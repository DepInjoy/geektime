/**
 * 21. 合并两个有序链表
 * https://leetcode.cn/problems/merge-two-sorted-lists/description/
*/
ListNode* mergeTwoLists(ListNode* list1, ListNode* list2) {
    if (!list1) return list2;
    if (!list2) return list1;

    ListNode* cur1 = list1, *cur2 = list2;
    ListNode dummy, *cur = &dummy;
    while (cur1 && cur2) {
        if (cur1->val <= cur2->val) {
            cur->next = cur1;
            cur1 = cur1->next;
        } else {
            cur->next = cur2;
            cur2 = cur2->next;
        }
        cur = cur->next;
    }
    while (cur1) cur->next = cur1, cur1 = cur1->next, cur = cur->next;
    while (cur2) cur->next = cur2, cur2 = cur2->next, cur = cur->next;

    return dummy.next;
}
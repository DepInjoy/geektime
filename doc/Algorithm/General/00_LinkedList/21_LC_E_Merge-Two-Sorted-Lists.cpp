/**
 * 21. 合并两个有序链表
 * https://leetcode.cn/problems/merge-two-sorted-lists/description/
*/
ListNode* mergeTwoLists(ListNode* list1, ListNode* list2) {
    ListNode* cur1 = list1, *cur2 = list2;
    ListNode* dummy = new ListNode(-1), *cur = dummy;
    while (cur1 && cur2) {
        if (cur1->val < cur2->val) {
            cur->next = cur1;
            cur = cur->next;
            cur1 = cur1->next;
        } else if (cur1->val > cur2->val) {
            cur->next = cur2;
            cur = cur->next;
            cur2 = cur2->next;
        } else {
            cur->next = cur1;
            cur1 = cur1->next;
            cur->next->next = cur2;
            cur2 = cur2->next;
            cur = cur->next->next;
        }
    }

    cur->next = cur1 ? cur1 : cur2;
    ListNode* ans = dummy->next;
    delete dummy;

    return ans;
}
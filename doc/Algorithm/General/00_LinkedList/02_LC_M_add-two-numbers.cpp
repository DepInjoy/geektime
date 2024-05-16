/**
 *  2. 两数相加
 *  https://leetcode.cn/problems/add-two-numbers/description/
 */

ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
    ListNode* head = nullptr, *tail = nullptr;
    int carry = 0;
    while (l1 || l2) {
        int sum = carry;
        if (l1) sum += l1->val, l1 = l1->next;
        if (l2) sum += l2->val, l2 = l2->next;
        if (!head) {
            head = tail = new ListNode(sum % 10);
        } else {
            tail->next = new ListNode(sum % 10);
            tail = tail->next;
        }
        carry = sum / 10;
    }
    if (carry) tail->next = new ListNode(carry);

    return head;
}

ListNode* addTwoNumbers(ListNode* l1, ListNode* l2) {
    ListNode dummy, *cur = &dummy;
    int carry = 0;
    while (l1 || l2) {
        int sum = carry;
        if (l1) sum += l1->val, l1 = l1->next;
        if (l2) sum += l2->val, l2 = l2->next;
        cur->next = new ListNode(sum % 10);
        carry = sum / 10;
        cur = cur->next;
    }
    if (carry) cur->next = new ListNode(carry);
    return dummy.next;
}
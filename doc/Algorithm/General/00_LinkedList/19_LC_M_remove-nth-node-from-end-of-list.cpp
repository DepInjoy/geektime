/**
 * 19. 删除链表的倒数第 N 个结点
 * https://leetcode.cn/problems/remove-nth-node-from-end-of-list/
*/

// 计算链表长度
ListNode* removeNthFromEnd(ListNode* head, int n) {
    int len = 0;
    ListNode* cur = head;
    while (cur) cur = cur->next, ++len;

    ListNode dummy(0, head);
    cur = &dummy;
    for (int i = 1; i <= len - n; ++i) {
        cur = cur->next;
    }
    cur->next = cur->next->next;
    return dummy.next;
}

// 利用栈,空间和时间复杂度O(N)
ListNode* removeNthFromEnd(ListNode* head, int n) {
    std::stack<ListNode*> stk;
    ListNode dummy(0, head);
    ListNode* cur = &dummy;
    while (cur) {
        stk.push(cur);
        cur = cur->next;
    }

    while (n--) stk.pop();

    ListNode* prev = stk.top();
    prev->next = prev->next->next;
    return dummy.next;
}

// 双指针
ListNode* removeNthFromEnd(ListNode* head, int n) {
    ListNode dummy(0, head);
    ListNode* cur = &dummy;
    ListNode* first = head, *second = &dummy;
    while (n--) first = first->next;
    while (first) {
        first = first->next;
        second =second->next;
    }
    second->next = second->next->next;
    return dummy.next;
}
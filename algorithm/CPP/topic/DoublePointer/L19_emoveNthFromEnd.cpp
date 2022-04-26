/**
 * @brief   给你一个链表，删除链表的倒数第 n 个结点，并且返回链表的头结点。
 *          链接：https://leetcode-cn.com/problems/remove-nth-node-from-end-of-list/
 * 
 */
struct ListNode {
    int val;
    ListNode *next;
    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

ListNode* removeNthFromEnd(ListNode* head, int n) {
    ListNode* dummyNode = new ListNode();
    dummyNode->next = head;

    ListNode* slow = dummyNode;
    ListNode* fast = head;
    for (int i = 0; i < n; i++) {
        fast = fast->next;
    }

    while (fast) {
        slow = slow->next;
        fast = fast->next;
    }

    ListNode* tmp = slow->next;
    slow->next = tmp->next;
    delete tmp;

    ListNode* ans = dummyNode->next;
    delete dummyNode;
    return ans->next;
}
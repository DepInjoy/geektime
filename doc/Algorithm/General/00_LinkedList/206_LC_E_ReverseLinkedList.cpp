/**
 * 206. 反转链表
 *  https://leetcode.cn/problems/reverse-linked-list/description/
 *  
 *  实现1: 虚拟出一个节点，假设对1->2->3->4进行反转，形成
 *
 *           nullptr -> 1  -> 2 -> 3 -> 4
 *              pre    cur
 *                     pre    cur
 *   将cur->next = pre，每次pre和cur向前移动一步
*/

// 迭代
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
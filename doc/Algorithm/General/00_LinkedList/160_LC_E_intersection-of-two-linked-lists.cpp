/**
 * 160. 相交链表
 * https://leetcode.cn/problems/intersection-of-two-linked-lists
 * 
 * 实现方式：
 *  1. Hash表+链表
 *  2. 双指针
*/

// Hash表存储
ListNode *getIntersectionNode(ListNode *headA, ListNode *headB) {
    std::unordered_set<ListNode*> visited;
    ListNode* node = headA;
    while (node) {
        visited.insert(node);
        node = node->next;
    }
    node = headB;
    while (node) {
        if (visited.count(node)) return node;
        node = node->next;
    }
    return nullptr;
}

// 双指针
ListNode *getIntersectionNode(ListNode *headA, ListNode *headB) {
    if (headA == nullptr || headB == nullptr)
        return nullptr;

    ListNode* pa = headA, *pb = headB;
    while (pa != pb) {
        pa = (pa == nullptr ? headB : pa->next);
        pb = (pb == nullptr ? headA : pb->next);
    }
    return pa;
}
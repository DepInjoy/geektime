/**
 *  23. 合并 K 个升序链表
 *  https://leetcode.cn/problems/merge-k-sorted-lists/description/
 * 
 *  实现方式：
 *    1. 优先队列(小顶推)
 */

// 优先队列(小顶推)
ListNode* mergeKLists(vector<ListNode*>& lists) {
    auto cmp = [](const ListNode* lhs, const ListNode* rhs) {
        return lhs->val > rhs->val;
    };

    std::priority_queue<ListNode*, std::vector<ListNode*>, decltype(cmp)> heap;
    ListNode dummy, *tail = &dummy;
    for (auto node : lists) {
        if (node) heap.push(node);
    }

    while (!heap.empty()) {
        auto cur = heap.top();
        heap.pop();
        tail->next = cur;
        tail = tail->next;
        if (cur->next) heap.push(cur->next);
    }
    return dummy.next;
}
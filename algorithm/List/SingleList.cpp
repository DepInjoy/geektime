#include <unordered_set>

/**
 * @brief 单向链表数据结构定义
 * 
 */
struct ListNode {
    int val;
    ListNode *next;

    ListNode() : val(0), next(nullptr) {}
    explicit ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};

/**
 * @brief           LeetCode-206:反转链表实现
 *                  实现的思想是将一个1->2->3->4->nullptr转化成nullptr->4->3->2->1的结构
 * 
 * @param head 
 * @return ListNode* 
 */
ListNode* reverseList(ListNode* head) {
    if (!head) return head;

    ListNode* cur = head;
    ListNode* pre = nullptr;
    while (cur) {
        ListNode* next = cur->next;
        cur->next = pre;
        pre = cur;
        cur = next;
    }
    return pre;
}

/**
 * @brief       LeetCode-141:环形链表(判断链表中是否有环)
 *              实现1. 通过快慢指针实现，快指针每次走两步，慢指针每次走一步
 *              实现2. 将遍历的指针存入unordered_set，当插入时元素已经存在，则表明有环，否则无环
 * 
 * @param head  链表头指针
 * @return bool 是否存在环
 */
bool hasCycle(ListNode *head) {
    if (!head || !head->next) return false;

    ListNode* slow = head;
    ListNode* fast = head->next;
    while (slow && slow->next && fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) return true;
    }
    return false;
}

bool hasCycle(ListNode *head) {
    std::unordered_set<ListNode*> nodes_set;
    ListNode* cur = head;
    while (cur) {
        if (!nodes_set.insert(cur).second) return true;
        cur = cur->next;
    }
    return false;
}

/**
 * @brief       LeetCode-24:两两交换链表中的节点
 *              实现1:如果只存在1到2个节点，则直接返回，否则，将头两个节点交互找到返回链表的head
 *                    之后开始循环，将两两节点交互
 *              
 *              实现2:虚拟出一个节点res，将其next指向head,假设原来是1->2->3->4,形成
 *                  res ->  1  ->  2  ->  3 ->4
 *                  pre    cur
 *                                pre    cur
 *                  不断循环,两两节点互换，之后将res->next返回即可。
 * 
 * @param head 
 * @return ListNode* 
 */
ListNode* swapPairs(ListNode* head) {
    if (!head || !head->next) return head;

    // 1. 将头两个节点互换，找到返回的链表head
    ListNode* res = head->next;
    head->next = head->next->next;
    res->next = head;
    // 2. 将之后的节点两两交互
    ListNode* pre = head;
    ListNode* cur = head->next;
    while (cur && cur->next) {
        ListNode* next = cur->next;
        cur->next = next->next;
        next->next = cur;
        pre->next = next;
        cur = cur->next;
        pre = pre->next->next;
    }
    return res;
}

ListNode* swapPairs(ListNode* head) {
    if (!head || !head->next) return head;

    ListNode* res = new ListNode(-1, head);
    ListNode* pre = res;
    ListNode* cur = head;
    while (cur && cur->next) {
        ListNode* next = cur->next;
        cur->next = next->next;
        next->next = cur;
        pre->next = next;
        cur = cur->next;
        pre = pre->next->next;
    }
    ListNode* t = res->next;
    delete res;
    return t;
}

/**
 * @brief       LeetCode-142:环形链表 II(返回链表开始入环的第一个节点,如果链表无环，则返回null)
 *              实现1: 链表中只有一个环节点，将节点依次存入set,插入失败的节点便是入环节点
 *              实现2: 双指针法
 * 
 * @param head 
 * @return ListNode* 
 */
ListNode *detectCycle(ListNode *head) {
    std::unordered_set<ListNode*> nodes_set;
    ListNode* cur = head;
    while (cur) {
        if (!nodes_set.insert(cur).second) return cur;
        cur = cur->next;
    }
    return nullptr;
}

ListNode *detectCycle(ListNode *head) {
    if (!head || !head->next) return nullptr;

    ListNode* slow = head;
    ListNode* fast = head;
    while (slow && slow->next && fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
        if (slow == fast) break;
    }
    // 无环, 直接return
    if (slow != fast) return nullptr;

    // 快指针指向head,快慢指针都每次移动一步
    // 快指针移动a步，慢指针移动a+nb步，两者相遇即换所在节点
    fast = head;
    while (slow != fast) {
        slow = slow->next;
        fast = fast->next;
    }
    return fast;
}
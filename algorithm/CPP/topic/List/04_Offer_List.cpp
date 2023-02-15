/**
 * @brief 
 * 
 */
#include <vector>

struct ListNode {
    int val;
    ListNode*next;
    ListNode(int x) : val(x), next(NULL) {}
};

/**
 * @brief 剑指Offer-24:反转链表
 *          定义一个函数，输入一个链表的头节点，反转该链表并输出反转后链表的头节点
 * 
 *          https://leetcode.cn/problems/fan-zhuan-lian-biao-lcof/description
 */
ListNode* reverseList(ListNode* head) {
    ListNode* pre = nullptr;
    while (head) {
        ListNode* tmp = head->next;
        head->next = pre;
        pre = head;
        head = tmp;
    }
    return pre;
}

/**
 * @brief 剑指Offer-06:从尾到头打印链表
 *              输入一个链表的头节点，从尾到头反过来返回每个节点的值（用数组返回）。
 *
 *      https://leetcode.cn/problems/cong-wei-dao-tou-da-yin-lian-biao-lcof/description/
 */
std::vector<int> reversePrint(ListNode* head) {
    std::vector<int> ans;
    ListNode* pre = nullptr;
    // 反转链表
    while (head) {
        ListNode* tmp = head->next;
        head->next = pre;
        pre = head;
        head = tmp;
    }

    while (pre) {
        ans.push_back(pre->val);
        pre = pre->next;
    }
    return ans;
}

/**
 * @brief 剑指Offer-35：复杂链表的复制
 *              请实现 copyRandomList 函数，复制一个复杂链表。
 *              在复杂链表中，每个节点除了有一个 next 指针指向下一个节点，
 *              还有一个 random 指针指向链表中的任意节点或者 null
 * 
 *         https://leetcode.cn/problems/fu-za-lian-biao-de-fu-zhi-lcof/description
 * 
 */
class Node {
public:
    int val;
    Node* next;
    Node* random;
    
    Node(int _val) {
        val = _val;
        next = NULL;
        random = NULL;
    }
};

Node* copyRandomList(Node* head) {
    if (!head) return nullptr;
    
}
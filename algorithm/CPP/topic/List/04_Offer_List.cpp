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

/**
 * @brief 剑指Offer-18:删除链表的节点
 *          给定单向链表的头指针和一个要删除的节点的值，定义一个函数删除该节点。
 *          返回删除后的链表的头节点。
 *      
 *      https://leetcode.cn/problems/shan-chu-lian-biao-de-jie-dian-lcof/description
 */
ListNode* deleteNode(ListNode* head, int val) {
    ListNode* ans = head->val == val ? head->next : head;
    ListNode* cur = ans;
    ListNode* pre = nullptr;
    while (cur) {
        if (cur->val == val) {
            ListNode* next = cur->next;
            pre->next = next;
            break;
        }
        pre = cur;
        cur = cur->next;
    }
    return ans;
}

/**
 * @brief   剑指Offer-22:链表中倒数第k个节点
 *              输入一个链表，输出该链表中倒数第k个节点。为了符合大多数人的习惯，
 *              本题从1开始计数，即链表的尾节点是倒数第1个节点。
 *              例如，一个链表有 6 个节点，从头节点开始，它们的值依次是 1、2、3、4、5、6。
 *              这个链表的倒数第 3 个节点是值为 4 的节点。
 * 
 *          https://leetcode.cn/problems/lian-biao-zhong-dao-shu-di-kge-jie-dian-lcof/description
 * 
 */

ListNode* getKthFromEnd(ListNode* head, int k) {
    int n = 0;
    ListNode* cur = head;
    while (cur) {
        ++n;
        cur = cur->next;
    }

    n -= k;
    while(n--) {
        head = head->next;
    }
    return head;
}

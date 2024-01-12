
## 数组

## 单链表
单向链表数据结构定义：
```C++
struct ListNode {
    int val;
    ListNode *next;

    ListNode() : val(0), next(nullptr) {}
    ListNode(int x) : val(x), next(nullptr) {}
    ListNode(int x, ListNode *next) : val(x), next(next) {}
};
```

### 练习题

- [LeetCode-24:两两交换链表中的节点](https://leetcode-cn.com/problems/swap-nodes-in-pairs/)
    - 实现1: 如果只存在1到2个节点，则直接返回，否则，将头两个节点交互找到返回链表的head, 之后开始循环，将两两节点交互
    - 实现2: 虚拟出一个节点res，将其`next`指向`head`,假设原来是`1->2->3->4`,形成
                  res ->  1  ->  2  ->  3 ->4
                  pre    cur
                                pre    cur
      不断循环,两两节点互换，之后将res->next返回即可。
- [LeetCode-142:环形链表 II](https://leetcode-cn.com/problems/linked-list-cycle-ii/)
    - 实现1: 链表中只有一个环节点，将节点依次存入set,插入失败的节点便是入环节点
    - 实现2：双指针法。
      假设链表共有`a+b`个节点，其中，链表头部到链表入口有`a`个节点(不计链表入口节点), 链表环有`b`个节点
      首先，慢指针每次走一步，快指针每次走两步，当两者相遇时，如果快指针比慢指针多走了`n`圈，即`nb`,而如果满指针走了`s`步，则快指针走了`2s`步，由此可知满指针走了`nb`步
      那么，将快指针移回到`head`，快慢指针都每次走一步，当快指针走`a`步时，慢指针走`a+nb`步，快慢指针将在此相遇。

- [LeetCode-25:K 个一组翻转链表](https://leetcode-cn.com/problems/reverse-nodes-in-k-group/)

## 双向链表

/**
 * @brief       面试题 04.03. 特定深度节点链表
 *              给定一棵二叉树，设计一个算法，创建含有某一深度上所有节点的链表
 *              （比如，若一棵树的深度为 D，则会创建出 D 个链表）。
 *              返回一个包含所有深度的链表的数组。
 *              链接：https://leetcode.cn/problems/list-of-depth-lcci/
 * 
 */

    vector<ListNode*> listOfDepth(TreeNode* tree) {
        if (!tree) return vector<ListNode*>();
        vector<ListNode*> ans;
        std::list<TreeNode*> cur{tree};
        while(cur.size()) {
            std::list<TreeNode*> nexts;
            ListNode* head = nullptr;
            ListNode* last = nullptr;
            while(!cur.empty()) {
                TreeNode* item = cur.front();
                if (item->left) nexts.push_back(item->left);
                if (item->right) nexts.push_back(item->right);
                if (!head) {
                    head = new ListNode(item->val);
                    last = head;
                } else {
                    last->next = new ListNode(item->val);
                    last = last->next;
                }
                cur.pop_front();
            }
            cur = nexts;
            ans.push_back(head);
        }
        return ans;
    }
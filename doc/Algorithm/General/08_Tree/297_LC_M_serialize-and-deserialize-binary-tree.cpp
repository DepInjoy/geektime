/**
 * 297. 二叉树的序列化与反序列化
 * https://leetcode.cn/problems/serialize-and-deserialize-binary-tree/description/
*/

// Encodes a tree to a single string.
void dfs(TreeNode* root, std::string& data) {
    if (!root) {
        data.append("#,");
        return;
    }

    data += std::to_string(root->val) + ",";
    dfs(root->left, data);
    dfs(root->right, data);
}

string serialize(TreeNode* root) {
    std::string data;
    dfs(root, data);
    return data;
}

// Decodes your encoded data to tree.
TreeNode* deserialize(std::list<std::string>& items) {
    auto cur = items.front();
    if (cur == "#") {
        items.erase(items.begin());
        return nullptr;
    }

    TreeNode* root = new TreeNode(std::stoi(cur));
    items.erase(items.begin());
    root->left = deserialize(items);
    root->right = deserialize(items);
    return root;
}

TreeNode* deserialize(string data) {
    std::string str;
    std::list<std::string> items;
    for (char ch : data) {
        if (ch == ',') {
            items.push_back(str);
            str.clear();
        } else {
            str.push_back(ch);
        }
    }
    TreeNode* root = deserialize(items);
    return root;
}
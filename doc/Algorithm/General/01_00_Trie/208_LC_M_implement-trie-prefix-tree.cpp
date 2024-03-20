/**
 * 208. 实现 Trie (前缀树)
 * https://leetcode.cn/problems/implement-trie-prefix-tree/description/
*/
#include <vector>
#include <string>

class Trie {
private:
    std::vector<Trie*> childern;
    bool isEnd{false};

    Trie* searchPrefix(const std::string& prefix) {
        Trie* node = this;
        for (char ch : prefix) {
            int index = ch - 'a';
            if (!node->childern[index]) {
                return nullptr;
            }
            node = node->childern[index];
        }
        return node;
    }
public:
    Trie() : childern(26), isEnd(false) {}
    
    void insert(string word) {
        Trie* node = this;
        for (char ch : word) {
            int index = ch - 'a';
            if (!node->childern[index]) {
                node->childern[index] = new Trie();
            }
            node = node->childern[index];
        }
        node->isEnd = true;
    }
    
    bool search(string word) {
        Trie* node = searchPrefix(word);
        return node && node->isEnd;
    }
    
    bool startsWith(string prefix) {
        return searchPrefix(prefix);
    }
};
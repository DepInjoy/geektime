#include <string>
#include <vector>
using namespace std;

/**
 *  剑指 Offer II 062. 实现前缀树
 *      Trie（发音类似 "try"）或者说 前缀树 是一种树形数据结构，用于高效地存储和检索字符串数据集中的键
 *      这一数据结构有相当多的应用情景，例如自动补完和拼写检查。
 *      
 * 请你实现 Trie 类：
 *      Trie()初始化前缀树对象。
 *      void insert(String word)向前缀树中插入字符串word
 *      boolean search(String word)如果字符串word在前缀树中，
 *          返回true(即，在检索之前已经插入);否则返回false
 *      boolean startsWith(String prefix)如果之前已经插入的字符串word的前缀之一为prefix
 *          返回true;否则,返回 false
*/
class Trie {
private:
    bool isEnd{false};
    std::vector<Trie*> children;//(26, nullptr);
    Trie* searchPrex(const std::string& word) {
        Trie* node = this;
        for (char ch : word) {
            if (node->children[ch - 'a'] == nullptr) {
                return nullptr;
            }
            node = node->children[ch - 'a'];
        }
        return node;
    }
public:
    /** Initialize your data structure here. */
    Trie() : children(26, nullptr), isEnd(false) {
    }

    /** Inserts a word into the trie. */
    void insert(string word) {
        Trie* node = this;
        for (char ch : word) {
            if (node->children[ch - 'a'] == nullptr) {
                node->children[ch - 'a'] = new Trie();
            }
            node = node->children[ch - 'a'];
        }
        node->isEnd = true;
    }

    /** Returns if the word is in the trie. */
    bool search(string word) {
        Trie* node = searchPrex(word);
        return (node && node->isEnd);
    }
    
    /** Returns if there is any word in the trie that starts with the given prefix. */
    bool startsWith(string prefix) {
        return searchPrex(prefix) != nullptr;
    }
};

/**
 *  剑指 Offer II 063. 替换单词
 *  https://leetcode.cn/problems/UhWRSj/description
 *      在英语中，有一个叫做词根(root)的概念, 它可以跟着其他一些词组成另一个较长的单词——我们称这个词为继承词(successor)。
 *      例如，词根an，跟随着单词 other(其他)，可以形成新的单词 another(另一个)。
 *      现在，给定一个由许多词根组成的词典和一个句子，需要将句子中的所有继承词用词根替换掉。
 *      如果继承词有许多可以形成它的词根，则用最短的词根替换它。需要输出替换之后的句子。
*/

// 方式一:利用unerdered_set进行求解
std::vector<std::string_view> split(const std::string& sentence, const char ch) {
    int start = 0, pos = 0;
    const int size = sentence.size();
    std::string_view view(sentence);
    std::vector<std::string_view> ans;
    while(pos < sentence.size()) {
        while(pos < size && sentence[pos] == ch) {
            ++pos;
        }
        start = pos;
        while (pos < size && sentence[pos] != ch) {
            ++pos;
        }
        if (start < size) {
            ans.push_back(view.substr(start, pos - start));
        }
    }
    return ans;
}

string replaceWords(vector<string>& dictionary, string sentence) {
    const int dsize = dictionary.size();
    // 根据dictionary中字符串的长度进行排序
    std::unordered_set<std::string_view> dset(dsize);
    for (std::string& ditem : dictionary) {
        dset.insert(ditem);
    }

    std::vector<std::string_view> words = split(sentence, ' ');
    for (auto& word : words) {
        for (int i = 0; i < word.size(); ++i) {
            if (dset.count(word.substr(0, i+1))) {
                word = word.substr(0, i+1);
                break;
            }
        }
    }

    std::string ans;
    for (int i = 0; i < words.size() - 1; ++i) {
        ans.append(words[i]);
        ans.append(" ");
    }
    ans.append(words.back());
    return ans;
}

// 方式2:前缀树求解，时间和空间复杂度均为为O(d+s)
// d = dictionary.size(), s = sentence.size()
class Trie {
private:
    bool isEnd{false};
    std::vector<Trie*> children;
public:
    Trie() : children(26, nullptr) {};
    void insert(const std::string& word) {
        Trie* node = this;
        for (char ch : word) {
            if (node->children[ch - 'a'] == nullptr) {
                node->children[ch - 'a'] = new Trie();
            }
            node = node->children[ch - 'a'];
        }
        node->isEnd = true;
    }

    std::string findPrex(const std::string_view& word) {
        Trie* node = this;
        std::string ans;
        for (char ch : word) {
            if (node->isEnd || node->children[ch - 'a'] == nullptr) {
                break;
            }
            ans.insert(ans.end(), ch);
            node = node->children[ch - 'a'];
        }
        return node->isEnd ? ans : ""; 
    }
};

std::vector<std::string_view> split(const std::string& sentence, const char ch) {
    int start = 0, pos = 0;
    const int size = sentence.size();
    std::string_view view(sentence);
    std::vector<std::string_view> ans;
    while(pos < sentence.size()) {
        while(pos < size && sentence[pos] == ch) {
            ++pos;
        }
        start = pos;
        while (pos < size && sentence[pos] != ch) {
            ++pos;
        }
        if (start < size) {
            ans.push_back(view.substr(start, pos - start));
        }
    }
    return ans;
}

string replaceWords(vector<string>& dictionary, string sentence) {
    Trie* trie = new Trie();
    for (auto& ditem : dictionary) {
        trie->insert(ditem);
    }

    std::vector<std::string_view> words = split(sentence, ' ');
    std::string ans;
    for (int i = 0; i < words.size() - 1; ++i) {
        std::string prex = trie->findPrex(words[i]);
        if (!prex.empty()) {
            ans.append(prex);
            ans.append(" ");
        } else {
            ans.append(words[i]);
                ans.append(" ");
        }
    }
    std::string prex = trie->findPrex(words.back());
    if (!prex.empty()) {
        ans.append(prex);
    } else {
        ans.append(words.back());
    }
    return ans;
}

/**
 * 剑指 Offer II 064. 神奇的字典
 *      https://leetcode.cn/problems/US1pGT/description/
*/
struct Trie {
    bool isEnd{false};
    std::vector<Trie*> children;
    Trie():children(26, nullptr) {}
};
class MagicDictionary {
private:
    Trie* trie;
public:
    /** Initialize your data structure here. */
    MagicDictionary() {
        trie = new Trie();
    }
    
    void buildDict(vector<string> dictionary) {
        for (std::string word : dictionary) {
            Trie* node = trie;
            for (char ch : word) {
                if (node->children[ch - 'a'] == nullptr) {
                    node->children[ch - 'a'] = new Trie();
                }
                node = node->children[ch - 'a'];
            }
            node->isEnd = true;
        }
    }
    
    bool search(string searchWord) {
        function<bool(Trie*, int, int)> dfs = [&](Trie* node, int pos, int edit) {
            if (node == nullptr) {
                return false;
            }

            if (pos == searchWord.size() && edit == 1 && node->isEnd) {
                return true;
            }

            if (pos < searchWord.size() && edit <= 1) {
                bool found = false;
                for (int j = 0; j < 26 && !found; ++j) {
                    int next = ((j == (searchWord[pos] - 'a')) ? edit : edit + 1);
                    found = dfs(node->children[j], pos + 1, next);
                }
                return found;
            }
            return false;
        };

        return dfs(trie, 0, false);
    }
};

/**
 *  剑指 Offer II 065. 最短的单词编码
 *      https://leetcode.cn/problems/iSwD2y/description/
 *      https://leetcode-cn.com/problems/short-encoding-of-words/
*/
// 字典树
struct Trie {
    std::vector<Trie*> children;
    int count;

    Trie() : children(26, nullptr), count(0) {}
    Trie* get(const char ch) {
        if (nullptr == children[ch - 'a']) {
            children[ch - 'a'] = new Trie();
            ++count;
        }
        return children[ch - 'a'];
    }
};

int minimumLengthEncoding(vector<string>& words) {
    const int n = words.size();
    Trie* trie = new Trie();
    std::unordered_map<Trie*, int> map(n);
    for (int i = 0; i < words.size(); ++i) {
        Trie* cur = trie;
        for (int j = words[i].size() - 1; j >= 0; --j) {
            cur = cur->get(words[i][j]);
        }
        map[cur] = i;
    }

    int ans = 0;
    for (auto iter : map) {
        if (iter.first->count == 0) {
            ans += words[iter.second].size() + 1;
        }
    }
    return ans;
}

// set储存
int minimumLengthEncoding(vector<string>& words) {
    int ans = 0;
    std::unordered_set<std::string> uset(words.begin(), words.end());
    for (auto& word : uset) {
        for (int i = 1; i < word.size(); ++i) {
            // 取后缀并尝试将其从unordered_set中删除
            uset.erase(word.substr(i));
        }
    }

    for (auto& word : uset) {
        ans += word.size() + 1;
    }
    return ans;
}
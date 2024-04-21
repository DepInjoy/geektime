/**
 * 212. 单词搜索 II
 * https://leetcode.cn/problems/word-search-ii/description/
*/

// 前缀树+DFS
struct Trie {
    Trie() : children(26), isEnd{false} {}
    void insert(const std::string& word) {
        Trie* node = this;
        for (char ch : word) {
            int index = ch - 'a';
            if (!node->children[index]) {
                node->children[index] = new Trie();
            }
            node = node->children[index];
        }
        node->isEnd = true;
        node->word = word;
    }
    std::vector<Trie*>  children;
    bool isEnd;
    std::string word;
};

bool dfs(vector<vector<char>>& board, Trie* node, int i, int j, std::unordered_set<std::string>& valid_words) {
    const int m = board.size(), n = board[0].size();
    char ch = board[i][j];
    if (!node->children[ch - 'a']) return false;

    node = node->children[ch - 'a'];
    if (node->isEnd) valid_words.insert(node->word);

    int dx[4] = {0, 1, 0, -1};
    int dy[4] = {1, 0, -1, 0};
    board[i][j] = '#';
    for (int k = 0; k < 4; ++k) {
        int x = i + dx[k], y = j + dy[k];
        if (x >= 0 && x < m && y >= 0 && y < n && board[x][y] != '#') {
            dfs(board, node, x, y, valid_words);
        }
    }
    board[i][j] = ch;
    return true;
}

vector<string> findWords(vector<vector<char>>& board, vector<string>& words) {
    Trie trie;
    for (auto word : words) {
        trie.insert(word);
    }

    std::unordered_set<std::string> valid_words;
    const int m = board.size(), n = board[0].size();
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            dfs(board, &trie, i, j, valid_words);
        }
    }

    std::vector<std::string> ans;
    ans.reserve(valid_words.size());
    for (auto word : valid_words) {
        ans.emplace_back(word);
    }
    return ans;
}
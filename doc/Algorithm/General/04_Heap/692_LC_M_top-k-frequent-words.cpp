/**
 * 692. 前K个高频单词
 * https://leetcode.cn/problems/top-k-frequent-words/description
*/



// 堆+Hash表
vector<string> topKFrequent(vector<string>& words, int k) {
    const int n = words.size();
    std::unordered_map<std::string, int> ump(n);
    for (std::string word : words) {
        ump[word]++;
    }

    auto cmp = [](const std::pair<std::string, int>& lhs, std::pair<std::string, int>& rhs) {
        if (rhs.second == lhs.second) {
            return lhs.first < rhs.first;
        }
        return lhs.second > rhs.second;
    };

    std::priority_queue<std::pair<std::string, int>, std::vector<std::pair<std::string, int>>, decltype(cmp)> heap(cmp);
    for (auto& [word, cnt] : ump) {
        heap.emplace(word, cnt);
        if (heap.size() > k) {
            heap.pop();
        }
    }


    std::vector<std::string> ans(heap.size());
    for (int i = heap.size() - 1; i >= 0; --i) {
        ans[i] = heap.top().first;
        heap.pop();
    }
    return ans;
}
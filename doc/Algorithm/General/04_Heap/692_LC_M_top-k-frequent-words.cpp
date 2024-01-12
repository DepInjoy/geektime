/**
 * 692. 前K个高频单词
 * https://leetcode.cn/problems/top-k-frequent-words/description
*/

//------------------------- 堆+Hash表 ----------------------------------
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
// ---------------------------------------------------------------------------

// -------------------------------- Hash表 + 快排 -----------------------------
using PII = std::pair<std::string, int>;
bool cmp(const PII& lhs, const PII& rhs) {
    return lhs.second == rhs.second ? lhs.first < rhs.first : lhs.second > rhs.second;
}

void quick_sort(std::vector<PII>& datas, int l, int r, int k) {
    if (l >= r) return;

    PII x = datas[random() % (r - l + 1) + l];
    int i = l - 1, j = r + 1;
    while (i < j) {
        while(cmp(datas[++i], x));
        while(cmp(x, datas[--j]));
        if (i < j) {
            std::swap(datas[i], datas[j]);
        }
    }

    quick_sort(datas, l, j, k), quick_sort(datas, j+1, r, k);
}

std::vector<std::string> topKFrequent(vector<string>& words, int k) {
    const int n = words.size();
    std::unordered_map<std::string, int> ump(n);
    for (auto word : words) {
        ump[word]++;
    }

    std::vector<PII> datas;
    datas.reserve(n);
    for (auto& [word, cnt] : ump) {
        datas.push_back({word, cnt});
    }

    int l = 0, r = datas.size() - 1;
    quick_sort(datas, l, r, k);

    std::vector<std::string> ans(k);
    for (int i = 0; i < k; ++i) {
        ans[i] = datas[i].first;
    }
    return ans;
}
// ---------------------------------------------------------------------------
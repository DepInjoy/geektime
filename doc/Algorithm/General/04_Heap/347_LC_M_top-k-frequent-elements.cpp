/**
 * 347. 前 K 个高频元素
 * https://leetcode.cn/problems/top-k-frequent-elements/description
*/

// ---------------------- 快排 + Hash表 ---------------------------
using PII = std::pair<int, int>;
bool cmp(const PII& lhs, const PII& rhs) {
    return lhs.second > rhs.second;
}

void quickSort(std::vector<PII>& data, int l, int r, int k) {
    if (l >= r) return;

    int i = l - 1, j = r + 1;
    PII x = data[random() % (r - l + 1) + l];
    while (i < j) {
        while (cmp(data[++i], x));
        while (cmp(x, data[--j]));
        if (i < j) std::swap(data[i], data[j]);
    }

    if (j - l + 1 >= k) {
        quickSort(data, l, j, k);
    } else {
        quickSort(data, j + 1, r, k - (j - l + 1));
    }
}

vector<int> topKFrequent(vector<int>& nums, int k) {
    std::unordered_map<int, int> freqs;
    for (int num : nums) ++freqs[num];

    std::vector<PII> data;
    const int n = freqs.size();
    data.reserve(freqs.size());
    for (auto item : freqs) data.emplace_back(item);

    quickSort(data, 0, n-1, k);

    std::vector<int> ans(k);
    for (int i = 0; i < k; ++i) ans[i] = data[i].first;
    return ans;
}
// ---------------------- 快排 + Hash表 ---------------------------

// 堆 + hash表
vector<int> topKFrequent(vector<int>& nums, int k) {
    auto cmp = [](const std::pair<int, int>& l, const std::pair<int, int>& r) {
        return l.second > r.second;
    };
    
    const int n = nums.size();
    std::unordered_map<int, int> ump(n);
    for (int num : nums) {
        ++ump[num];
    }

    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, decltype(cmp)> heap(cmp);
    for (auto& [num, cnt] : ump) {
        heap.emplace(num, cnt);
        if (heap.size() > k) {
            heap.pop();
        }
    }

    std::vector<int> ans(heap.size());
    for (int i = heap.size() - 1; i >= 0; --i) {
        ans[i] = heap.top().first;
        heap.pop();
    }
    return ans;
}
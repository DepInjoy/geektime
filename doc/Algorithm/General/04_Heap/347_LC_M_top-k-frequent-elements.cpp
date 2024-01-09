/**
 * 347. 前 K 个高频元素
 * https://leetcode.cn/problems/top-k-frequent-elements/description
*/

// ---------------------- 快排 + Hash表 ---------------------------
bool cmp(const std::pair<int, int>& l, const std::pair<int, int>& r) {
    return l.second > r.second;
}

void quick_sort(std::vector<std::pair<int, int>>& data, int l , int r, int k) {
    if (l >= r) return;
    
    std::pair<int, int> x = data[rand() % (r - l + 1) + l];
    int i = l - 1, j = r + 1;
    while (i < j) {
        while(cmp(data[++i], x));
        while(cmp(x, data[--j]));
        if (i < j) std::swap(data[i],data[j]);
    }

    if (j -l + 1 > k) {
        quick_sort(data, l, j, k);
    } else {
        quick_sort(data, j + 1, r, k - (j - l + 1));
    }
}

vector<int> topKFrequent(vector<int>& nums, int k) {
    const int n = nums.size();
    std::unordered_map<int, int> ump(n);
    for (int num : nums) {
        ump[num]++;
    }

    const int dn = ump.size();
    std::vector<std::pair<int, int>> data(dn);
    int i = 0;
    for (auto& [val, cnt] : ump) {
        data[i++] = std::make_pair<>(val, cnt);
    }

    quick_sort(data, 0, dn - 1, k);
    std::vector<int> ans(k);
    for (int i = 0; i < k; i++) {
        ans[i] = data[i].first;
    }
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
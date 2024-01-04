/**
 * 347. 前 K 个高频元素
 * https://leetcode.cn/problems/top-k-frequent-elements/description
*/

static bool cmp(std::pair<int, int>& l, std::pair<int, int>& r) {
    return r.second < l.second;
};

vector<int> topKFrequent(vector<int>& nums, int k) {
    std::unordered_map<int, int> ump;
    for (int num : nums) {
        ++ump[num];
    }

    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>, decltype(&cmp)> prio(cmp);

    for (auto& [num, cnt] : ump) {
        if (prio.size() == k) {
            if (prio.top().second < cnt) {
                prio.pop();
                prio.emplace(num, cnt);
            }
        } else {
            prio.emplace(num, cnt);
        }
    }

    std::vector<int> ans(prio.size());
    int i = 0;
    while (!prio.empty()) {
        ans[i++] = prio.top().first;
        prio.pop();
    }
    return ans;
}
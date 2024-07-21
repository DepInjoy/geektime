/**
 * 239. 滑动窗口最大值
 * https://leetcode.cn/problems/sliding-window-maximum
 * 
 * 实现方式：
 *  1. 优先队列
 *  2. 单调队列
 *  3. 分块+预处理
 */


// 优先队列
vector<int> maxSlidingWindow(vector<int>& nums, int k) {
    const int n = nums.size();
    std::priority_queue<std::pair<int, int>> q;
    for (int i = 0; i < k - 1; ++i) {
        q.push({nums[i], i});
    }        

    std::vector<int> ans;
    ans.reserve(n-k+1);
    for (int i = k - 1; i < n; ++i) {
        q.push({nums[i], i});
        while (q.top().second <= i - k) {
            q.pop();
        }
        ans.emplace_back(q.top().first);
    }
    return ans;
}

// 单调队列
    vector<int> maxSlidingWindow(vector<int>& nums, int k) {
        std::deque<int> q;
        std::vector<int> ans;
        ans.reserve(nums.size()-k+1);
        for (int i = 0; i < nums.size(); i++) {
            if (!q.empty() && i - q.front() + 1 > k) q.pop_front();
            while(!q.empty() && nums[q.back()] <= nums[i]) q.pop_back();
            q.push_back(i);

            if (i >= k-1) ans.push_back(nums[q.front()]);
        }
        return ans;
    }
    
vector<int> maxSlidingWindow(vector<int>& nums, int k) {
    std::deque<int> q;
    const int n = nums.size();
    for (int i = 0; i < k - 1; ++i) {
        while (!q.empty() && nums[q.back()] <= nums[i]) {
            q.pop_back();
        }
        q.push_back(i);
    }

    std::vector<int> ans;
    ans.reserve(n-k+1);
    for (int i = k - 1; i < n; ++i) {
        // 窗口内保持单调性
        while (!q.empty() && nums[q.back()] <= nums[i]) {
            q.pop_back();
        }
        q.push_back(i);

        // 将不再窗口中的值弹出
        while (q.front() <= i - k) {
            q.pop_front();
        }
        ans.emplace_back(nums[q.front()]);
    }
    return ans;
}

// 分块+预处理
vector<int> maxSlidingWindow(vector<int>& nums, int k) {
    const int n = nums.size();
    std::vector<int> preMaxs(n), suffixMax(n);
    for (int i = 0; i < n; ++i) {
        if (i % k == 0) {
            preMaxs[i] = nums[i];
        } else {
            preMaxs[i] = std::max(preMaxs[i-1], nums[i]);
        }
    }

    for (int i = n - 1; i >= 0; --i) {
        if (i == n - 1 || ((i + 1) % k == 0)) {
            suffixMax[i] = nums[i];
        } else {
            suffixMax[i] = std::max(suffixMax[i+1], nums[i]);
        }
    }

    std::vector<int> ans;
    ans.reserve(n-k+1);
    for (int i = 0; i <= n-k; ++i) {
        ans.push_back(std::max(preMaxs[i+k-1], suffixMax[i]));
    }
    return ans;
}
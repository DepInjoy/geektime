[104. 二叉树的最大深度](https://leetcode.cn/problems/maximum-depth-of-binary-tree/)

```C++
// 深度优先搜索
int maxDepth(TreeNode* root) {
    if (!root) return 0;
    return std::max(maxDepth(root->left), maxDepth(root->right)) + 1;
}

// 广度优先搜索
    int maxDepth(TreeNode* root) {
        if (!root) return 0;
        int ans = 0;
        std::queue<TreeNode*> q;
        q.push(root);
        while (!q.empty()) {
            int cnt = q.size();
            while (cnt--) {
                TreeNode* cur = q.front();
                q.pop();
                if (cur->left) q.push(cur->left);
                if (cur->right) q.push(cur->right);
            }
            ++ans;
        }
        return ans;
    }
```





[1768. 交替合并字符串](https://leetcode.cn/problems/merge-strings-alternately/)

```C++
    string mergeAlternately(string word1, string word2) {
        const int m = word1.size(), n = word2.size();
        int i = 0, j = 0;
        std::string ans;
        ans.reserve(m+n);
        while ((i < m) && (j < n)) {
            ans.push_back(word1[i++]);
            ans.push_back(word2[j++]);
        }
        while (i < m) ans.push_back(word1[i++]);
        while (j < n) ans.push_back(word2[j++]);
        return ans;
    }
```

[1679. K 和数对的最大数目](https://leetcode.cn/problems/max-number-of-k-sum-pairs/)

```C++
int maxOperations(vector<int>& nums, int k) {
    int ans = 0;
    std::unordered_map<int, int> vi_map;
    for (int i = 0; i < nums.size(); ++i) {
        int left = k - nums[i];
        auto item = vi_map.find(left);
        if (item != vi_map.end() && vi_map[left]) {
            ++ans;
            --vi_map[left];
        } else {
            ++vi_map[nums[i]];
        }
    }
    return ans;
}
```

[2215. 找出两数组的不同](https://leetcode.cn/problems/find-the-difference-of-two-arrays/)

```C++
vector<vector<int>> findDifference(vector<int>& nums1, vector<int>& nums2) {
    std::unordered_set<int> unique1(nums1.begin(), nums1.end());
    std::unordered_set<int> unique2(nums2.begin(), nums2.end());
    std::vector<std::vector<int>> ans(2, std::vector<int>());
    for (auto item : unique1) {
        if (!unique2.count(item)) ans[0].push_back(item);
    }

    for (auto item : unique2) {
        if (!unique1.count(item)) ans[1].push_back(item);
    }
    return ans;
}
```

[1431. 拥有最多糖果的孩子](https://leetcode.cn/problems/kids-with-the-greatest-number-of-candies/)

```C++
vector<bool> kidsWithCandies(vector<int>& candies, int extraCandies) {
    const int n = candies.size();
    std::vector<bool> ans(n);
    int maxCandies = *std::max_element(candies.begin(), candies.end());
    for (int i = 0; i < n; ++i) {
        ans[i] = (candies[i] + extraCandies >= maxCandies);
    }
    return ans;
}
```

[206. 反转链表](https://leetcode.cn/problems/reverse-linked-list/)

```C++
ListNode* reverseList(ListNode* head) {
    ListNode* pre = nullptr;
    ListNode* cur = head;
    while (cur) {
        ListNode* tmp = cur->next;
        cur->next = pre;
        pre = cur;
        cur = tmp;
    }
    return pre;
}
```

[1071. 字符串的最大公因子](https://leetcode.cn/problems/greatest-common-divisor-of-strings/)

```C++
// 枚举
bool check(const std::string& sub, const std::string str) {
    bool valid = true;
    const int len = sub.size();
    int times = str.size() / len;
    for (int i = 0; i < times && valid; ++i) {
        for (int j = 0; j < len && valid; ++j) {
            valid = (sub[j] == str[j + i * len]);
        }
    }
    return valid;
}

string gcdOfStrings(string str1, string str2) {
    int l1 = str1.size(), l2 = str2.size();
    for (int i = std::min(l1, l2); i >= 1; --i) {
        if (l1 % i == 0 && l2 % i == 0) {
            std::string substr = str1.substr(0, i);
            if (check(substr, str1) && check(substr, str2)) {
                return substr;
            }
        }
    }
    return "";
}

// 枚举优化(l1和l2最大公因数)
string gcdOfStrings(string str1, string str2) {
    int l1 = str1.size(), l2 = str2.size();
    std::string substr = str1.substr(0, __gcd(l1, l2));
    return (check(substr, str1) && check(substr, str2)) ? substr : "";
}

// 数学法
string gcdOfStrings(string str1, string str2) {
    return (str1 + str2 != str2 + str1) ? "" :
        str1.substr(0, __gcd(str1.size(), str2.size()));
}
```

[345. 反转字符串中的元音字母](https://leetcode.cn/problems/reverse-vowels-of-a-string/)

```C++
// 双指针
string reverseVowels(string s) {
    std::unordered_set<char> yy {'a', 'e', 'i', 'o', 'u', 'A', 'E', 'I', 'O', 'U'};
    int l = 0, r = s.size() - 1;
    const int n = s.size();
    while (l < r) {
        while(l < n && yy.find(s[l]) == yy.end()) ++l;
        while(r >= 0 && yy.find(s[r]) == yy.end()) --r;
        if (l < r) std::swap(s[l++], s[r--]);
    }
    return s;
}
```

[2095. 删除链表的中间节点](https://leetcode.cn/problems/delete-the-middle-node-of-a-linked-list/)

```C++
// 快慢指针
ListNode* deleteMiddle(ListNode* head) {
    if (!head->next) return nullptr;

    ListNode* slow = head, *fast = head, *pre = nullptr;
    while (fast && fast->next) {
        fast = fast->next->next;
        pre = slow;
        slow = slow->next;
    }
    pre->next = slow->next;
    return head;
}

// 两次遍历
ListNode* deleteMiddle(ListNode* head) {
    if (!head->next) return nullptr;

    int pos = 0;
    ListNode* cur = head, *pre = head;
    while (cur) {
        ++pos;
        cur = cur->next;
    }

    pos /= 2;
    cur = head;
    while (pos--) {
        pre = cur;
        cur = cur->next;
    }
    pre->next = cur->next;
    return head;
}
```

[1207. 独一无二的出现次数](https://leetcode.cn/problems/unique-number-of-occurrences/)

```C++
bool uniqueOccurrences(vector<int>& arr) {
    std::unordered_map<int, int> umap;
    std::unordered_set<int> uset;
    for (int i = 0; i < arr.size(); ++i) {
        ++umap[arr[i]];
    }

    for (auto item : umap) {
        if (uset.count(item.second)) {
            return false;
        }
        uset.insert(item.second);
    }
    return true;
}
```

[283. 移动零](https://leetcode.cn/problems/move-zeroes/)

```C++
void moveZeroes(vector<int>& nums) {
    int l = 0, r = 0;
    const int n = nums.size();
    while (r < n) {
        if (nums[r]) {
            std::swap(nums[l++], nums[r]);
        }
        ++r;
    }
}
```

[392. 判断子序列](https://leetcode.cn/problems/is-subsequence/)

```C++
bool isSubsequence(string s, string t) {
    const int m = s.size(), n = t.size();
    int i = 0, j = 0;
    while (i < m && j < n) {
        if (s[i] == t[j++]) {
            ++i;
        }
    }
    return i == m;
}
```

[11. 盛最多水的容器](https://leetcode.cn/problems/container-with-most-water/)

```C++
// 双指针,时间复杂度O(N)，空间复杂度O(1)
int maxArea(vector<int>& height) {
    const int n = height.size();
    int maxArea = 0, l = 0, r = height.size() - 1;
    while (l < r) {
        maxArea = std::max(maxArea, std::min(height[l], height[r]) * (r - l));
        if (height[l] <= height[r]) {
            ++l;
        } else {
            --r;
        }
    }
    return maxArea;
}
```

[1732. 找到最高海拔](https://leetcode.cn/problems/find-the-highest-altitude/)

```C++
int largestAltitude(vector<int>& gain) {
    int largest = 0, altitude = 0;
    for (auto item : gain) {
        altitude += item;
        largest = std::max(largest, altitude);
    }
    return largest;
}
```

[724. 寻找数组的中心下标](https://leetcode.cn/problems/find-pivot-index/)

```C++
// 前缀和
int pivotIndex(vector<int>& nums) {
    int total = std::accumulate(nums.begin(), nums.end(), 0);
    int sum = 0;
    for (int i = 0; i < nums.size(); ++i) {
        if (2 * sum + nums[i] == total) {
            return i;
        }
        sum += nums[i];
    }
    return -1;
}
```

[238. 除自身以外数组的乘积](https://leetcode.cn/problems/product-of-array-except-self/)

```C++
vector<int> productExceptSelf(vector<int>& nums) {
    const int n = nums.size();
    std::vector<int> ans(n);
    ans[0] = 1;

    for (int i = 1; i < n; ++i) {
        ans[i] = ans[i-1] * nums[i-1];
    }

    int rProduct = 1;
    for (int i = n - 1; i >= 0; --i) {
        ans[i] *= rProduct;
        rProduct *= nums[i];
    }
    return ans;
}
```

[643. 子数组最大平均数 I](https://leetcode.cn/problems/maximum-average-subarray-i/)

```C++
// 滑动窗口
double findMaxAverage(vector<int>& nums, int k) {
    const int n = nums.size();
    double maxSum = 0, sum = 0;
    for (int i = 0; i < k; ++i) {
        sum += nums[i];
    }

    maxSum = sum;
    for (int i = k; i < n; ++i) {
        sum += (nums[i] - nums[i - k]);
        maxSum = std::max(maxSum, sum);
    }
    return maxSum / k;
}
```

[1456. 定长子串中元音的最大数目](https://leetcode.cn/problems/maximum-number-of-vowels-in-a-substring-of-given-length/)

```C++
int maxVowels(string s, int k) {
    const std::unordered_set<char> uset{'a', 'e', 'i', 'o', 'u'};
    int cnt = 0, maxcnt = 0;
    for (int i = 0; i < k; ++i) {
        if (uset.count(s[i])) ++cnt;
    }

    maxcnt = cnt;
    for (int i = k; i < s.size(); ++i) {
        cnt += (uset.count(s[i]) - uset.count(s[i - k]));
        maxcnt = std::max(maxcnt, cnt);
    }
    return maxcnt;
}
```

[933. 最近的请求次数](https://leetcode.cn/problems/number-of-recent-calls/)

```C++
class RecentCounter {
public:
    RecentCounter() {}
    
    int ping(int t) {
        q.push(t);
        while (q.front() < t - 3000) {
            q.pop();
        }
        return q.size();
    }
private:
    std::queue<int> q;
};
```

[328. 奇偶链表](https://leetcode.cn/problems/odd-even-linked-list/)

```C++
ListNode* oddEvenList(ListNode* head) {
    if (!head) return head;

    ListNode* eventHead = head->next;
    ListNode* odd = head, *event = eventHead;
    while (event && event->next) {
        odd->next = event->next;
        odd = event->next;
        event->next = odd->next;
        event = event->next;
    }
    odd->next = eventHead;
    return head;
}
```

[2130. 链表最大孪生和](https://leetcode.cn/problems/maximum-twin-sum-of-a-linked-list/)

```C++
int pairSum(ListNode* head) {
    // 快慢指针找到中间节点
    ListNode* slow = head, *fast = head->next;
    while (fast && fast->next) {
        slow = slow->next;
        fast = fast->next->next;
    }

    // 链表翻转
    ListNode* lastHalf = nullptr, *cur = slow;
    while (cur) {
        ListNode* tmp = cur->next;
        cur->next = lastHalf;
        lastHalf = cur;
        cur = tmp;
    }

    int ans = INT_MIN;
    while (lastHalf && head) {
        ans = std::max(ans, lastHalf->val + head->val);
        lastHalf = lastHalf->next;
        head = head->next;
    }
    return ans;
}
```

[338. 比特位计数](https://leetcode.cn/problems/counting-bits/)

```C++
// Brian Kernighan算法
int bits(int x) {
    int cnt = 0;
    while (x > 0) {
        x &= (x - 1);
        ++cnt;
    }
    return cnt;
}
vector<int> countBits(int n) {
    std::vector<int> ans(n+1);
    for (int i = 1; i <= n; ++i) {
        ans[i] = bits(i);
    }
    return ans;
}


```

[136. 只出现一次的数字](https://leetcode.cn/problems/single-number/)

```C++
int singleNumber(vector<int>& nums) {
    int ans = 0;
    for (auto item : nums) {
        ans ^= item;
    }
    return ans;
}
```

[1318. 或运算的最小翻转次数](https://leetcode.cn/problems/minimum-flips-to-make-a-or-b-equal-to-c/)

```C++
// 枚举+位运算
int minFlips(int a, int b, int c) {
    int times = 0;
    for (int i = 0; i < 32; ++i) {
        int bit_a = (a & (1 << i)) >> i; // (a >> i) & 1
        int bit_b = (b & (1 << i)) >> i;
        int bit_c = (c & (1 << i)) >> i;
        times += (bit_c ? (bit_a + bit_b == 0) : (bit_a + bit_b));
    }
    return times;
}
```

[208. 实现 Trie (前缀树)](https://leetcode.cn/problems/implement-trie-prefix-tree/)

```C++
class Trie {
public:
    Trie() : children(26) {}
    
    void insert(string word) {
        Trie* node = this;
        for (char ch: word) {
            ch -= 'a';
            if (node->children[ch] == nullptr) {
                node->children[ch] = new Trie();
            }
            node = node->children[ch];
        }
        node->isEnd = true;
    }
    
    bool search(string word) {
        Trie* node = searchPrexfix(word);
        return node && node->isEnd;
    }
    
    bool startsWith(string prefix) {
        Trie* node = searchPrexfix(prefix);
        return node;
    }

private:
    std::vector<Trie*> children;
    bool isEnd {false};

    Trie* searchPrexfix(string word) {
        Trie* node = this;
        for (char ch : word) {
            ch -= 'a';
            if (node->children[ch] == nullptr) {
                return nullptr;
            }
            node = node->children[ch];
        }
        return node;
    }
};
```

[62. Unique Paths](https://leetcode.cn/problems/unique-paths/description)

```C++
// 动态规划
int uniquePaths(int m, int n) {
    std::vector<std::vector<int>> dp(m, std::vector<int>(n));
    for (int i = 0; i < m; ++i) {
        dp[i][0] = 1;
    }

    for (int j = 1; j < n; ++j) {
        dp[0][j] = 1;
    }

    for (int i = 1; i < m; ++i) {
        for (int j = 1; j < n; ++j) {
            dp[i][j] = dp[i-1][j] + dp[i][j-1];
        }
    }
    return dp[m-1][n-1];
}

// 动态规划+滚动数组
int uniquePaths(int m, int n) {
    std::vector<int> dp(n);
    for (int j = 0; j < n; ++j) {
        dp[j] = 1;
    }

    for (int i = 1; i < m; ++i) {
        for (int j = 1; j < n; ++j) {
            dp[j] += dp[j-1];
        }
    }
    return dp[n-1];
}

// 组合数学

```

[1143. 最长公共子序列](https://leetcode.cn/problems/longest-common-subsequence/description/)
```C++
int longestCommonSubsequence(string text1, string text2) {
    const int m = text1.size(), n = text2.size();
    std::vector<std::vector<int>> dp(m+1, std::vector<int>(n+1));
    for (int i = 1; i <= m; ++i) {
        for (int j = 1; j <= n; ++j) {
            if (text1[i-1] == text2[j-1]) {
                dp[i][j] = dp[i-1][j-1] + 1;
            } else {
                dp[i][j] = std::max(dp[i-1][j], dp[i][j-1]);
            }
        }
    }
    return dp[m][n];
}
```

[714. 买卖股票的最佳时机含手续费](https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-with-transaction-fee/description/)

```C++
// 动态规划
int maxProfit(vector<int>& prices, int fee) {
    const int n = prices.size();
    std::vector<std::vector<int>> dp(n, std::vector<int>(2));
    dp[0][0] = 0;
    dp[0][1] = -prices[0];
    for (int i = 1; i < n; ++i) {
        dp[i][0] = std::max(dp[i-1][0], dp[i-1][1] + prices[i] - fee);
        dp[i][1] = std::max(dp[i-1][1], dp[i-1][0] - prices[i]);
    }
    return dp[n-1][0];
}

// 贪心算法
int maxProfit(vector<int>& prices, int fee) {
    int buy = prices[0] + fee, profit = 0;
    for (int i = 1; i < prices.size(); ++i) {
        if (prices[i] + fee < buy) {
            buy = prices[i] + fee;
        } else if (prices[i] > buy) {
            profit += prices[i] - buy;
            buy = prices[i];
        }
    }
    return profit;
}
```

<<<<<<< Updated upstream
[605. 种花问题](https://leetcode.cn/problems/can-place-flowers/description)

```C++
    bool canPlaceFlowers(vector<int>& flowerbed, int n) {
        int pre = -1, cnt = 0;
        const int m = flowerbed.size();
        for (int j = 0; j < m; ++j) {
            if (flowerbed[j]) {
                if (pre == -1) {
                    cnt += j >> 1;
                } else {
                    cnt +=  (j - pre - 2) >> 1;
                }

                if (cnt >= n) return true;
                pre = j;
            }
        }

        if (pre == -1) {
            // 如果没有任何花朵,则有m个位置可以种花,最多可以种(m+1)/2朵花
            cnt += (m + 1) >> 1;
        } else {
            cnt += (m - pre - 1) >> 1;
        }
        return cnt >= n;
    }
```

[\334. 递增的三元子序列](https://leetcode.cn/problems/increasing-triplet-subsequence/description)

```C++
bool increasingTriplet(vector<int>& nums) {
    int first = nums[0], second = INT_MAX;
    for (int i = 1; i < nums.size(); ++i) {
        if (nums[i] > second) {
            return true;
        } else if (nums[i] <= first) {
            first = nums[i];
        } else {
            second = nums[i];
        }
    }
    return false;
}
```

[443. 压缩字符串](https://leetcode.cn/problems/string-compression/description)

```C++
int compress(vector<char>& chars) {
    int i = 0, write = 0;
    const int n = chars.size();
    for (int j = 0; j < n; ++j) {
        if (j == n -1 || chars[j] != chars[j+1]) {
            chars[write++] = chars[j];
            int cnt = (j - i + 1);
            if (cnt > 1) {
                int start = write;
                while (cnt > 0) {
                    chars[write++] = (cnt % 10) + '0';
                    cnt /= 10;
                }
                std::reverse(chars.begin() + start, chars.begin() + write);
            }
            i = j + 1;
        }
    }
    return write;
}
```

=======
[124. 二叉树中的最大路径和](https://leetcode.cn/problems/binary-tree-maximum-path-sum/description/)
```C++
int maxPathSum(TreeNode* root, int& maxSum) {
    if (!root) return 0;
    int leftMax = std::max(0, maxPathSum(root->left, maxSum));
    int rightMax = std::max(0, maxPathSum(root->right, maxSum));
    maxSum = std::max(maxSum, root->val + leftMax + rightMax);
    return root->val + std::max(leftMax, rightMax);
}

int maxPathSum(TreeNode* root) {
    int maxSum = INT_MIN;
    maxPathSum(root, maxSum);
    return maxSum;
}
```
[1493. 删掉一个元素以后全为 1 的最长子数组](https://leetcode.cn/problems/longest-subarray-of-1s-after-deleting-one-element/description)

```C++
// 递推
int longestSubarray(vector<int>& nums) {
    const int n = nums.size();
    std::vector<int> pre(n), suf(n);
    pre[0] = nums[0], suf[n-1] = nums[n-1];
    for (int i = 1; i < n; ++i) {
        pre[i] = (nums[i] == 1 ? pre[i-1] + 1 : 0);
    }

    for (int i = n - 2; i >= 0; --i) {
        suf[i] = (nums[i] == 1 ? suf[i + 1] + 1 : 0);
    }

    int ans  = 0;
    for (int i = 0; i < n; ++i) {
        int preSum = (i == 0 ? 0 : pre[i-1]);
        int sufSum = (i == n - 1 ? 0 : suf[i+1]);
        ans = std::max(ans, preSum + sufSum);
    }
    return ans;
}

// 优化递推
int longestSubarray(vector<int>& nums) {
    int pre = 0, cur = 0, ans = 0;
    for (auto num : nums) {
        if (num == 1) {
            ++pre, ++cur;
        } else {
            cur = pre;
            pre = 0;
        }
        ans = std::max(ans, cur);
    }

    if (ans == nums.size()) --ans;
    return ans;
}
```
>>>>>>> Stashed changes

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


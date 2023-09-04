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
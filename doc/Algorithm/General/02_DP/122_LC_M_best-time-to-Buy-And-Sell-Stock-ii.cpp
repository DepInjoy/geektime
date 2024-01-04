/**
 * 122. 买卖股票的最佳时机 II
 * https://leetcode.cn/problems/best-time-to-buy-and-sell-stock-ii/description/
*/

/**
 * 定义状态
 *      dp[i][0]表示第i天交易完后手里没有股票的最大利润
 *      dp[i][1]表示第i天交易完后手里持有一支股票的最大利润
 * 
 * 状态转移方程
 *      dp[i][0]=max{dp[i−1][0], dp[i−1][1]+prices[i]}
 *      dp[i][1]=max{dp[i−1][1], dp[i−1][0]−prices[i]}
*/

int maxProfit(vector<int>& prices) {
    const int n = prices.size();
    std::vector<std::vector<int>> dp(n, std::vector<int>(2));
    dp[0][0] = 0, dp[0][1] = -prices[0];
    for (int i = 1; i < n; ++i) {
        dp[i][0] = std::max(dp[i-1][0], dp[i-1][1] + prices[i]);
        dp[i][1] = std::max(dp[i-1][1], dp[i-1][0] - prices[i]);
    }

    return dp[n-1][0];
}

// 空间压缩+DP
int maxProfit(vector<int>& prices) {
    const int n = prices.size();
    
    std::vector<std::vector<int>> dp(2, std::vector<int>(2));
    dp[0][0] = 0, dp[0][1] = -prices[0];
    for (int i = 1; i < n; ++i) {
        int j = (i-1) % 2;
        dp[i%2][0] = std::max(dp[j][0], dp[j][1] + prices[i]);
        dp[i%2][1] = std::max(dp[j][1], dp[j][0] - prices[i]);
    }
    return dp[(n-1)%2][0];
}
/**
 * 518. 零钱兑换 II
 * https://leetcode.cn/problems/coin-change-ii/description/
*/

// 完全背包
int change(int amount, vector<int>& coins) {
    std::vector<int> dp(amount+1);
    dp[0] = 1;
    for (int i = 0; i < coins.size(); ++i) {
        for (int j = coins[i]; j <= amount; ++j) {
            dp[j] += dp[j - coins[i]];
        }
    }
    return dp[amount];
}
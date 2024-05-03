/**
 * 121. 买卖股票的最佳时机
 * https://leetcode.cn/problems/best-time-to-buy-and-sell-stock/description/
*/

int maxProfit(vector<int>& prices) {
    int minprice = INT_MAX, maxpro = 0;
    for (int price : prices) {
        maxpro = std::max(maxpro, price - minprice);
        minprice = std::min(price, minprice);
    }
    return maxpro;
}

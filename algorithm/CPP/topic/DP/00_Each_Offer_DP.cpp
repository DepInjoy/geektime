/***/

/**
 * @brief [121] 买卖股票的最佳时机
 *          状态定义:
 *              prein：表示当前持有的最低买入价格
 *              maxpro：表示当前最大利润
 */

int maxProfit(vector<int>& prices) {
    int prein = INT_MAX, maxpro = 0;
    for (int price : prices) {
        prein = std::min(prein, price);
        maxpro = std::max(maxpro, price - prein);
    }
    return maxpro;
}

/**
 * @brief  [123] 买卖股票的最佳时机 III
 * 
 */
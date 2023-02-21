/**
*/


/**
 * @brief   剑指 Offer 10- I. 斐波那契数列
 *          写一个函数，输入 n ，求斐波那契（Fibonacci）数列的第 n 项（即F(N)）。
 *          斐波那契数列的定义如下：
 *          
 *              F(0) = 0,   F(1) = 1
 *              F(N) = F(N - 1) + F(N - 2), 其中 N > 1
 *          答案需要取模 1e9+7(1000000007)，如计算初始结果为：1000000008，请返回 1。
 * 
 *          https://leetcode.cn/problems/fei-bo-na-qi-shu-lie-lcof/description/
 * 
 */

int fib(int n) {
    if (n == 0 || n == 1) return n;

    const int MOD = 1000000007;
    int prepre = 0, pre = 1, cur = 1;
    while (n-- > 1) {
        cur = (prepre + pre) % MOD;
        prepre = pre;
        pre = cur;
    }
    return cur;
}

/**
 * @brief   剑指 Offer 10- II. 青蛙跳台阶问题
 *          一只青蛙一次可以跳上1级台阶，也可以跳上2级台阶。
 *          求该青蛙跳上一个n级的台阶总共有多少种跳法。
 *          答案需要取模 1e9+7(1000000007，如计算初始结果为：1000000008，请返回1。
 * 
 *          https://leetcode.cn/problems/qing-wa-tiao-tai-jie-wen-ti-lcof/description
 */

int numWays(int n) {
    if (n == 0) return 1;
    if (n == 1) return n;

    const int MOD = 1e9+7;
    int prepre = 1, pre = 1, cur = 2;
    while (n-- > 1) {
        cur = (prepre + pre) % MOD;
        prepre = pre;
        pre = cur;
    }
    return cur;
}

/**
 * @brief   剑指Offer-63:股票的最大利润
 *              假设把某股票的价格按照时间先后顺序存储在数组中，
 *              请问买卖该股票一次可能获得的最大利润是多少？
 *          https://leetcode.cn/problems/gu-piao-de-zui-da-li-run-lcof/description
 */

    int maxProfit(vector<int>& prices) {
        if (prices.size() < 2) return 0;

        int in = INT_MAX, maxProfit = 0;
        for (int i = 0; i < prices.size();++i) {
            in = std::min(in, prices[i]);
            maxProfit = std::max(maxProfit, prices[i] - in);
        }
        return maxProfit; 
    }
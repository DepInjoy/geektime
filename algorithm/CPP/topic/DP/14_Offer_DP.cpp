#include <vector>
#include <string>

using namespace std;


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
    int in = INT_MAX, maxProfit = 0;
    for (int i = 0; i < prices.size();++i) {
        in = std::min(in, prices[i]);
        maxProfit = std::max(maxProfit, prices[i] - in);
    }
    return maxProfit; 
}

/**
 * @brief   剑指Offer-42:连续子数组的最大和
 *              输入一个整型数组，数组中的一个或连续多个整数组成一个子数组。
 *              求所有子数组的和的最大值。
 *              要求时间复杂度为O(n)。
 * 
 *          https://leetcode.cn/problems/lian-xu-zi-shu-zu-de-zui-da-he-lcof/description
 * 
 *          f[i]表示以第i个数字结尾的子数组的最大和，那么
 *              f[i] = max(f[i-1] + data[i], f[i])
 *          其中，f[0] = data[0]
 */
int maxSubArray(vector<int>& nums) {
    int pre = 0, maxAns = nums[0];
    for (int num : nums) {
        pre = std::max(pre + num, num);
        maxAns = std::max(maxAns, pre);
    }
    return maxAns;
}

/**
 * @brief 剑指Offer-47:礼物的最大价值
 *          在一个 m*n 的棋盘的每一格都放有一个礼物，每个礼物都有一定的价值（价值大于 0）。
 *          你可以从棋盘的左上角开始拿格子里的礼物，并每次向右或者向下移动一格、直到到达棋
 *          盘的右下角。给定一个棋盘及其上面的礼物的价值，请计算你最多能拿到多少价值的礼物？
 * 
 *          https://leetcode.cn/problems/li-wu-de-zui-da-jie-zhi-lcof/description
 */

// 空间复杂度为O(m)，时间复杂度为(m*n)
int maxValue(vector<vector<int>>& grid) {
    int m = grid.size(), n = grid[0].size();
    std::vector<int> values(n);
    values[0] = grid[0][0];
    for (int i = 1; i < n; i++) {
        values[i] = values[i-1] + grid[0][i];
    }

    for (int i = 1; i < m; ++i) {
        values[0] += grid[i][0];
        for (int j = 1; j < n; ++j) {
            values[j] = std::max(values[j], values[j-1]) + grid[i][j];
        }
    }
    return values[n-1];
}

// 空间复杂度为o(min(m ,n)), 时间复杂度为(m*n)
int maxValue(vector<vector<int>>& grid) {
    int m = grid.size(), n = grid[0].size();
    // 是否列数更小
    bool ml = (m <= n);
    // 行数m和列数n之间较小的值
    int ln = (ml ? m : n);
    int gn = (ml ? n : m);
    std::vector<int> values(ln);
    values[0] = grid[0][0];
    for (int i = 1; i < ln; i++) {
        values[i] = values[i-1] + (ml ? grid[i][0] : grid[0][i]);
    }

    for (int i = 1; i < gn; ++i) {
        values[0] += (ml ?  grid[0][i] : grid[i][0]);
        for (int j = 1; j < ln; ++j) {
            values[j] = std::max(values[j], values[j-1]) + (ml ? grid[j][i] : grid[i][j]);
        }
    }
    return values[ln-1];
}

/**
 * @brief 剑指Offer46:把数字翻译成字符串
 *          给定一个数字，我们按照如下规则把它翻译为字符串：0 翻译成 “a” ，
 *          1 翻译成 “b”，……，11 翻译成 “l”，……，25 翻译成 “z”。
 *          一个数字可能有多个翻译。请编程实现一个函数，用来计算一个数字有多少种不同的翻译方法。
 * 
 *          https://leetcode.cn/problems/ba-shu-zi-fan-yi-cheng-zi-fu-chuan-lcof/description/
 */

int translateNum(int num) {
    int new_num = 0;
    while (num) {
        new_num = new_num * 10 + (num % 10);
        num = num / 10;
    }

    std::vector<int> arr(32);
    return arr[0];
}

/**
 * @brief 剑指Offer-48:最长不含重复字符的子字符串
 *          请从字符串中找出一个最长的不包含重复字符的子字符串，计算该最长子字符串的长度。
 * 
 *          https://leetcode.cn/problems/zui-chang-bu-han-zhong-fu-zi-fu-de-zi-zi-fu-chuan-lcof/description
 * 
 */

int lengthOfLongestSubstring(string s) {
    std::vector<int> arr(s.size());
    
}
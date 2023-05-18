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

/**
 * @brief   剑指 Offer 14-I. 剪绳子
 *              给你一根长度为 n 的绳子，请把绳子剪成整数长度的 m 段（m、n都是整数，n>1并且m>1），
 *              每段绳子的长度记为 k[0],k[1]...k[m-1]. 请问 k[0]*k[1]*...*k[m-1]可能的最大乘积是多少？
 *              例如，当绳子的长度是8时，我们把它剪成长度分别为2、3、3的三段，此时得到的最大乘积是18
 * 
 *          https://leetcode.cn/problems/jian-sheng-zi-lcof/description
 */

/**
 * @brief   dp[i] 表示将正整数 iii 拆分成至少两个正整数的和之后，这些正整数的最大乘积
 * 
 */
int cuttingRope(int n) {
    std::vector<int> products(n+1, 0);
    products[0]= 0, products[1] = 0;
    for (int i = 2; i <= n; ++i) {
        for (int j = 1; j < i; ++j) {
            products[i] = std::max(products[i], std::max(j *(i - j), j * products[i - j]));;
        }
    }
    return products[n];
}

/**
 *  专题 088. 爬楼梯的最少成本
 *  https://leetcode.cn/problems/GzCJIP/description
 *  https://leetcode.cn/problems/min-cost-climbing-stairs/
 * 
 *      数组的每个下标作为一个阶梯，第 i 个阶梯对应着一个非负数的体力花费值 cost[i]（下标从 0 开始）。
 *      每当爬上一个阶梯都要花费对应的体力值，一旦支付了相应的体力值，就可以选择向上爬一个阶梯或者爬两个阶梯。
 *      请找出达到楼层顶部的最低花费。在开始时，你可以选择从下标为 0 或 1 的元素作为初始阶梯。
 * 
 *  假设数组cost的长度为n，则n个阶梯分别对应下标0到 n−1，
 *  楼层顶部对应下标n，问题等价于计算达到下标n的最小花费。
 * 
 *  dp[i]表示到达下标i的最少成本
 *  状态转移方程: dp[i] = min(dp[i-1] + cost[i-1], dp[i-2] + cost[i-2]), 其中2 <= 0 <= n
 *  初始化状态: dp[0] = 0, dp[1] = 0;
*/

int minCostClimbingStairs(vector<int>& cost) {
    int pre = 0, cur = 0;
    for (int i = 2; i <= cost.size(); ++i) {
        int next = std::min(pre + cost[i-2], cur + cost[i-1]);
        pre = cur;
        cur = next;
    }
    return cur;
}

/**
 * 专题 剑指 Offer-II 089. 房屋偷盗(中等)
 *      https://leetcode.cn/problems/Gu0c2T/description/
 * 
 *      一个专业的小偷，计划偷窃沿街的房屋。每间房内都藏有一定的现金，
 *      影响小偷偷窃的唯一制约因素就是相邻的房屋装有相互连通的防盗系统，
 *      如果两间相邻的房屋在同一晚上被小偷闯入，系统会自动报警。
 *      给定一个代表每个房屋存放金额的非负整数数组 nums
 *      请计算 不触动警报装置的情况下 ，一夜之内能够偷窃到的最高金额。
 *      
 *      dp[i]表示第一间房屋可以偷窃到的最高金额
 *      状态转移方程: dp[i] = max(dp[i-1] + nums[i], dp[i-1]) 
 *      初始化状态 dp[0] = num[0], dp[1] = max(nums[0], nums[1])
*/
int rob(vector<int>& nums) {
    int prepre = 0, pre = nums[0];
    for (int i = 1; i < nums.size(); ++i) {
        int cur = std::max(prepre + nums[i], pre);
        prepre = pre;
        pre = cur;
    }
    return pre;
}

/**
 *      剑指 Offer II 090. 环形房屋偷盗
 *      https://leetcode.cn/problems/PzWKhm/description/
 *      https://leetcode-cn.com/problems/house-robber-ii/
 * 
 *      一个专业的小偷，计划偷窃一个环形街道上沿街的房屋，每间房内都藏有一定的现金。
 *      这个地方所有的房屋都 围成一圈 ，这意味着第一个房屋和最后一个房屋是紧挨着的。
 *      同时，相邻的房屋装有相互连通的防盗系统，如果两间相邻的房屋在同一晚上被小偷闯入，系统会自动报警。
 *      给定一个代表每个房屋存放金额的非负整数数组 nums ，请计算 在不触动警报装置的情况下 ，今晚能够偷窃到的最高金额。
*/
int rob(vector<int>& nums) {
    // 偷第一个,不偷最后一个
    int prepre = 0, pre = nums[0];
    for (int i = 1; i < nums.size() - 1; ++i) {
        int cur = std::max(prepre + nums[i], pre);
        prepre = pre;
        pre = cur;
    }

    // 不偷第一个, 偷最后一个
    int prepre2 = 0, pre2 = 0;
    for (int i = 1; i < nums.size(); ++i) {
        int cur = std::max(prepre2 + nums[i], pre2);
        prepre2 = pre2;
        pre2 = cur;
    }
    return std::max(pre, pre2);
}

/**
 * 剑指 Offer II 091. 粉刷房子
 *      https://leetcode.cn/problems/JEj789/description/
 * 
 *      dp[i][0] = min(dp[i-1][1], dp[i-1][2]) + cost[i][j];
 *      dp[i][1] = min(dp[i-1][0], dp[i-1][2]) + cost[i][j];
 *      dp[i][2] = min(dp[i-1][0], dp[i-1][1]) + cost[i][j];
 * 
 *      dp[i][j] = min(dp[i-1][(j+1)%3], dp[i-1][(j+2)%3]) + cost[i][j];
*/

int minCost(vector<vector<int>>& costs) {
    const int n = costs.size();
    std::vector<std::vector<int>> dp(2, std::vector<int>(3));
    for (int i = 0; i < 3; ++i) {
        dp[0][i] = costs[0][i];
    }

    for (int i = 1; i < n; ++i) {
        const int k = (i-1) %2;
        for (int j = 0; j < 3; ++j) {
            dp[i % 2][j] = std::min(dp[k][(j+1)%3], dp[k][(j+2)%3]) + costs[i][j];
        }
    }
    return std::min(std::min(dp[(n - 1) % 2][0], dp[(n - 1) % 2][1]), dp[(n - 1) % 2][2]);
}

/**
 *  剑指 Offer II 092. 翻转字符
 *      https://leetcode.cn/problems/cyJERH/description/
 *      如果一个由 '0' 和 '1' 组成的字符串，是以一些 '0'（可能没有 '0'）后面跟着一些 '1'（也可能没有 '1'）的形式组成的，
 *      那么该字符串是单调递增的。我们给出一个由字符 '0' 和 '1' 组成的字符串 s，我们可以将任何 '0' 翻转为 '1' 或者将 '1' 翻转为 '0'。
 *      返回使 s 单调递增 的最小翻转次数。
 * 
 *      单调递增的字符串满足以下性质：
 *        1. 首个字符是 000 或 111
 *        2. 其余的每个字符，字符 000 前面的相邻字符一定是 000，字符 111 前面的相邻字符可以是 000 或 111
 *      dp[i][0]表示dp[i][0]分别表示下标i处的字符为0和1的情况下使得 s[0..i]单调递增的最小翻转次数
 * 
 *      dp[i][0] = dp[i-1][0] + I(s[i]='1')
 *      dp[i][1] = min(dp[i-1][0], dp[i-1]) + I(s[i]='0')
*/
int minFlipsMonoIncr(string s) {
    int dp0 = 0, dp1 = 0;
    for (char ch : s) {
        int next_dp0 = dp0 + (ch == '1' ? 1 : 0);
        int next_dp1 = std::min(dp0, dp1) + (ch == '0' ? 1 : 0);
        dp0 = next_dp0, dp1 = next_dp1;
    }
    return std::min(dp0, dp1);
}
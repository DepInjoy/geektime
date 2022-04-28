/**
 * @brief   给你一个整数数组cost，其中cost[i]是从楼梯第i个台阶向上爬需要支付的费用
 *          一旦你支付此费用，即可选择向上爬一个或者两个台阶。
 *          你可以选择从下标为 0 或下标为 1 的台阶开始爬楼梯。
 *          请你计算并返回达到楼梯顶部的最低花费
 *          链接：https://leetcode-cn.com/problems/min-cost-climbing-stairs
 * 
 *          实现思想：
 *              1. 动态规划+记忆化搜索
 *              2. 空间压缩
 *  
 */

#include <vector>
#include <algorithm>
#include <cmath>

using namespace std;

int minCostClimbingStairs(vector<int>& cost) {
    int N = cost.size();
    vector<int> dp = vector<int>(N+1);
    dp[0] = dp[1] = 0;
    for (int i = 2; i <= N; i++) {
        dp[i] = min(dp[i-1]+cost[i-1], dp[i-2]+cost[i-2]);
    }
    return dp[N];
}

// 空间压缩
int minCostClimbingStairs(vector<int>& cost) {
    int cur = 0, pre = 0, prepre = 0;
    for (int i = 2; i <= cost.size(); i++) {
        cur = min(pre+cost[i-1], prepre+cost[i-2]);
        prepre = pre;
        pre = cur;
    }
    return cur;
}
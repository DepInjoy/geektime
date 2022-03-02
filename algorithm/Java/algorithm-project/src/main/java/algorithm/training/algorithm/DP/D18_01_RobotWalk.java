package algorithm.training.algorithm.DP;

/**
 * @brief:      假设有排成一行的N个位置，记为1~N，N 一定大于或等于 2
 *              开始时机器人在其中的M位置上(M 一定是 1~N 中的一个)
 *              如果机器人来到1位置，那么下一步只能往右来到2位置；
 *              如果机器人来到N位置，那么下一步只能往左来到 N-1 位置；
 *              如果机器人来到中间位置，那么下一步可以往左走或者往右走；
 *              规定机器人必须走 K 步，最终能来到P位置(P也是1~N中的一个)的方法有多少种
 *              给定四个参数 N、M、K、P，返回方法数。
 *
 *              假设N=5 K=6 ami=4, 数据填充过程，其中行代表当前坐在位置cur，列表示剩余步骤rest
 *              按照rest列进行数据填充
 *                  0   1   2   3   4   5   6
 *              0   0
 *              1   0   0   0   1   0   0   0   2.      在1位置,只能是从2位置来到1, dp[1][rest]=dp[2][rest-1]
 *              2   0   x
 *              3   0   x                               dp[cur][rest] = dp[cur-1][rest-1] + dp[cur+1][rest-1]
 *              4   1   x
 *              5   0                                   对于N位置,只能是从N-1位置来到N, dp[N][rest]=dp[N-1][rest-1]
 *                  1.
 * */
public class D18_01_RobotWalk {
    public static int ways1(int N, int start, int aim, int K) {
        if(N < 2 || start < 1 || start > N || aim < 1 || aim > N || K < 1) return -1;
        return process(start, K, aim, N);
    }

    public static int process(int cur, int rest, int aim, int N) {
        // 没有步数可走,目前在aim方法数为1,否则为0
        if (rest == 0) {
            return  cur == aim ? 1 : 0;
        }

        // 等价于if (cur < 1 || cur > N) return 0;  // 如果越界了, 则方法数为0种
        // 这样的方式便于改写成DP
        if (cur == 1) return process(2, rest -1, aim, N);
        if (cur == N) return process(N-1, rest-1, aim, N);

        return process(cur + 1, rest - 1, aim, N) +
                process(cur - 1, rest - 1, aim, N);
    }

    public static int ways2(int N, int start, int aim, int K) {
        if(N < 2 || start < 1 || start > N || aim < 1 || aim > N || K < 1) return -1;

        int[][] dp = new int[N+1][K+1];
        // 剩余步数为0时，只有aim位置值为1,其他都为0
        dp[aim][0] = 1;
        for(int rest = 1; rest <= K; rest++) {
            dp[1][rest] = dp[2][rest - 1];
            for (int cur = 2; cur < N; cur++) {
                dp[cur][rest] = dp[cur+1][rest-1] + dp[cur-1][rest-1];
            }
            dp[N][rest] = dp[N-1][rest-1];
        }
        return dp[start][K];
    }

    public static void main(String[] args) {
        System.out.println(ways1(5, 2, 4, 6));
        System.out.println(ways2(5, 2, 4, 6));
        // System.out.println(ways3(5, 2, 4, 6));
    }
}

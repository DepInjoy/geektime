package algorithm.training.algorithm.DP;

import com.sun.org.apache.bcel.internal.generic.ARETURN;

/**
 * @brief:      请同学们自行搜索或者想象一个象棋的棋盘，
 *              然后把整个棋盘放入第一象限，棋盘的最左下角是(0,0)位置
 *              那么整个棋盘就是横坐标上9条线、纵坐标上10条线的区域
 *              给你三个 参数 x，y，k, 返回“马”从(0,0)位置出发，必须走k步
 *              最后落在(x,y)上的方法数有多少种?
 *
 *              数据填充过程，假设x=7, y=7, k=10
 *              当rest为0时，只有(7, 7)点为1，其他都为0
 *              0   0   0   0   0   0   0   0   0   0
 *              0   0   0   0   0   0   0   0   0   0
 *              0   0   0   0   0   0   0   0   0   0
 *              0   0   0   0   0   0   0   0   0   0
 *              0   0   0   0   0   0   0   0   0   0
 *              0   0   0   0   0   0   0   0   0   0
 *              0   0   0   0   0   0   0   0   0   0
 *              0   0   0   0   0   0   1   0   0   0
 *              0   0   0   0   0   0   0   0   0   0
 *              0   0   0   0   0   0   0   0   0   0
 *              之后依次填充1~10层的数据
 * */
public class D20_02_HorseJump {
    public static int jump(int x, int y, int k) {
        if (x < 0 || x > 9 || y < 0 || y > 8 || k < 0) return 0;

        return process(0, 0, x, y,  k);
    }

    // 动态规划实现
    public static int jump1(int x, int y, int k) {
        if (x < 0 || x > 9 || y < 0 || y > 8 || k < 0) return 0;

        int[][][] dp = new int[10][9][k+1];
        dp[x][y][0] = 1; // 剩下0步，只有当前在(x,y)位置有一种走法其他都是0种
        // 当前在(i,j)位置剩余rest步走到(0,0)位置可能的走法
        for (int rest = 1; rest <= k; rest++) {
            for (int i = 0; i < 10; i++) {
                for (int j = 0; j < 9; j++) {
                    dp[i][j][rest] = pick(dp,i + 1, j + 2, rest - 1);
                    dp[i][j][rest] += pick(dp,i + 1, j - 2, rest - 1);
                    dp[i][j][rest] += pick(dp,i - 1, j + 2, rest - 1);
                    dp[i][j][rest] += pick(dp,i - 1, j - 2, rest - 1);
                    dp[i][j][rest] += pick(dp,i + 2, j + 1, rest - 1);
                    dp[i][j][rest] += pick(dp,i + 2, j - 1, rest - 1);
                    dp[i][j][rest] += pick(dp,i - 2, j + 1, rest - 1);
                    dp[i][j][rest] += pick(dp,i - 2, j - 1, rest - 1);
                }
            }
        }
        return dp[0][0][k];
    }

    public static int pick(int[][][] dp, int cx, int cy, int rest) {
        return (cx < 0 || cx > 9 || cy < 0 || cy > 8) ? 0 : dp[cx][cy][rest];
    }

    // 返回当前在(cx, cy)剩余rest步到达(x, y)的方法数
    public static int process(int cx, int cy, int x, int y, int rest) {
        if (rest == 0) {
            return cx == x && cy == y ? 1 : 0;
        }

        if (cx < 0 || cx > 9 || cy < 0 || cy > 8) return 0;

        int ways = process(cx + 1, cy + 2, x, y, rest - 1);
        ways += process(cx + 1, cy - 2, x, y, rest - 1);
        ways += process(cx - 1, cy + 2, x, y, rest - 1);
        ways += process(cx - 1, cy - 2, x, y, rest - 1);
        ways += process(cx + 2, cy + 1, x, y, rest - 1);
        ways += process(cx + 2, cy - 1, x, y, rest - 1);
        ways += process(cx - 2, cy + 1, x, y, rest - 1);
        ways += process(cx - 2, cy - 1, x, y, rest - 1);
        return ways;
    }


    public static void main(String[] args) {
        int x = 7;
        int y = 7;
        int step = 10;

        System.out.println(jump(x, y, step));
        System.out.println(jump1(x, y, step));
    }
}

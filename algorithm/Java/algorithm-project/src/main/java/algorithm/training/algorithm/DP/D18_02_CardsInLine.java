package algorithm.training.algorithm.DP;

/**
 * @brief:      给定一个整型数组arr，代表数值不同的纸牌排成一条线
 *              玩家A和玩家B依次拿走每张纸牌, 规定玩家A先拿，玩家B后拿
 *              但是每个玩家每次只能拿走最左或最右的纸牌
 *              玩家A和玩家B都绝顶聪明, 请返回最后获胜者的分数。
 *
 *              数据填充过程，对于[5, 7, 4, 5, 8]
 *              先手,根据递归过程,当前值取决于后手左方和右方的值,可以按照斜对角线进行填充
 *                  0   1   2   3   4
 *              0   5   x
 *              1       7   x
 *              2           4   x
 *              3               5   x
 *              4                   8
 *
 *              后手:
 *                  0   1   2   3   4
 *              0   0   x
 *              1       0   x
 *              2           0   x
 *              3               0   x
 *              4                   0
 * */
public class D18_02_CardsInLine {
    public static int win1(int[] arr) {
        if (arr == null || arr.length < 1) return 0;

        int N = arr.length - 1;
        int p1 = first(arr, 0, N);
        int p2 = last(arr, 0, N);
        return Math.max(p1, p2);
    }

    public static int win2(int[] arr) {
        if (arr == null || arr.length < 1) return 0;

        int N = arr.length - 1;
        int[][] fdp = new int[N+1][N+1];
        int[][] ldp = new int[N+1][N+1];
        for (int i = 0; i <= N;i++) {
            fdp[i][i] = arr[i];
            ldp[i][i] = 0;
        }

        // 按照斜对角线的方向进行数据填充
        for(int col = 1; col <= N; col++) {
            int L = 0, R = col;
            while (R <= N) {
                fdp[L][R] = Math.max(arr[L]+ldp[L+1][R], arr[R]+ldp[L][R-1]);
                ldp[L][R] = Math.min(fdp[L+1][R], fdp[L][R-1]);
                L++;
                R++;
            }
        }
        return Math.max(fdp[0][N], ldp[0][N]);
    }

    public static int first(int[] arr, int L, int R) {
        if (L == R) return arr[L];

        int p1 = arr[L] + last(arr, L + 1, R);
        int p2 = arr[R] + last(arr, L, R - 1);
        return Math.max(p1, p2);
    }

    public static int last(int[] arr, int L, int R) {
        if (L == R) return 0;

        int p1 = first(arr, L + 1, R);
        int p2 = first(arr, L, R - 1);
        return Math.min(p1, p2);
    }

    public static void main(String[] args) {
        int[] arr = { 5, 7, 4, 5, 8, 1, 6, 0, 3, 4, 6, 1, 7 };
        System.out.println(win1(arr));
        System.out.println(win2(arr));
        // System.out.println(win3(arr));
    }
}

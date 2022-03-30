package algorithm.practice.dc.week2;

/**
 * @brief:      现有司机N*2人，调度中心会将所有司机平分给A、B两个区域
 *              第i个司机去A可得收入为income[i][0]，
 *              第 i 个司机去B可得收入为income[i][1]，
 *              返回所有调度方案中能使所有司机总收入最高的方案，是多少钱
 * */
public class DC_02_04_Drive {
    public static int maxMoney1(int[][] income) {
        return 0;
    }

    public static int maxMoney2(int[][] income) {
        return 0;
    }

    public static int maxMoney3(int[][] income) {
        return 0;
    }

    /**************************** For Test ********************/
    // 返回随机len*2大小的正数矩阵,值在0~value-1之间
    public static int[][] randomMatrix(int len, int value) {
        int[][] ans = new int[len << 1][2];
        for (int i = 0; i < ans.length; i++) {
            ans[i][0] = (int) (Math.random() * value);
            ans[i][1] = (int) (Math.random() * value);
        }
        return ans;
    }

    public static void main(String[] args) {
        int N = 10;
        int value = 100;
        int testTime = 500;
        System.out.println("测试开始");
        for (int i = 0; i < testTime; i++) {
            int len = (int) (Math.random() * N) + 1;
            int[][] matrix = randomMatrix(len, value);
            int ans1 = maxMoney1(matrix);
            int ans2 = maxMoney2(matrix);
            int ans3 = maxMoney3(matrix);
            if (ans1 != ans2 || ans1 != ans3) {
                System.out.println(ans1);
                System.out.println(ans2);
                System.out.println(ans3);
                System.out.println("Oops!");
            }
        }
        System.out.println("测试结束");
    }
}

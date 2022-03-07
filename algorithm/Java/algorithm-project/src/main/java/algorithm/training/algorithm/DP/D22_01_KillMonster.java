package algorithm.training.algorithm.DP;
/**
 * @brief:      给定3个参数，N，M，K, 怪兽有N滴血，等着英雄来砍自己
 *              英雄每一次打击，都会让怪兽流失[0~M]的血量
 *              到底流失多少？每一次在[0~M]上等概率的获得一个值
 *              求K次打击之后，英雄把怪兽砍死的概率
 * */
public class D22_01_KillMonster {
    public static double right(int N, int M, int K) {
        return 0.0;
    }

    public static double dp1(int N, int M, int K) {
        return 0.0;
    }

    public static double dp2(int N, int M, int K) {
        return 0.0;
    }

    /****************** For Test *************************/
    public static void main(String[] args) {
        int NMax = 10;
        int MMax = 10;
        int KMax = 10;
        int testTime = 200;
        System.out.println("测试开始");
        for (int i = 0; i < testTime; i++) {
            int N = (int) (Math.random() * NMax);
            int M = (int) (Math.random() * MMax);
            int K = (int) (Math.random() * KMax);
            double ans1 = right(N, M, K);
            double ans2 = dp1(N, M, K);
            double ans3 = dp2(N, M, K);
            if (ans1 != ans2 || ans1 != ans3) {
                System.out.println("Oops!");
                break;
            }
        }
        System.out.println("测试结束");
    }
}

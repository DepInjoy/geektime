package algorithm.training.algorithm.DP;

/**
 * @brief:      给定5个参数，N，M，row，col，k
 *              表示在N*M的区域上，醉汉Bob初始在(row,col)位置
 *              Bob一共要迈出k步，且每步都会等概率向上下左右四个方向走一个单位
 *              任何时候Bob只要离开N*M的区域，就直接死亡
 *              返回k步之后，Bob还在N*M的区域的概率
 * */
public class D21_05_BobDie {
    public static double livePosibility1(int row, int col, int k, int N, int M) {
        return 0;
    }

    public static double livePosibility2(int row, int col, int k, int N, int M) {
        return 0;
    }

    public static void main(String[] args) {
        System.out.println(livePosibility1(6, 6, 10, 50, 50));
        System.out.println(livePosibility2(6, 6, 10, 50, 50));
    }
}

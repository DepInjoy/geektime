package algorithm.training.algorithm.DP;

/**
 * @brief:                  从左往右的尝试模型 -> 动态规划
 *              给定两个长度都为N的数组weights和values
 *                  其中，weights[i]和values[i]分别代表 i号物品的重量和价值。
 *              给定一个正数bag，表示一个载重bag的袋子，装的物品不能超过这个重量。
 *              返回你能装下最多的价值是多少?
 * */
public class C19_01_Knapsack {
    /**
     * @brief:      左往右的尝试模型暴力尝试
     *              主要借助process函数，从左到右地根据是否将当前索引加入是否会使得价值增加,不断尝试
     * */
    public static int tryMaxValue(Integer[] w, Integer[] v, Integer bag) {
        if (w == null || v == null || w.length == 0 || v.length == 0 || bag <= 0) return 0;
        return processs(w, v, 0, bag);
    }

    public static Integer processs(Integer[] w, Integer[] v, Integer index, Integer rest) {
        if (index == w.length) return -1;
        if (rest <= 0) return -1;

        Integer p1 = processs(w, v, index + 1, rest);
        Integer p2 = 0;
        Integer next = processs(w, v, index + 1, rest - w[index]);
        if (next != -1) {
            p2 = v[index] + next;
        }
        return Math.max(p1, p2);
    }


    public static void main(String[] args) {
        Integer[] weights = {3, 2, 4, 7, 3, 1, 7};
        Integer[] values = {5, 6, 3, 19, 12, 4, 2};
        Integer bag = 15;
        Integer maxPrice = tryMaxValue(weights, values, bag);
        System.out.println(maxPrice);
    }
}

package algorithm.training.algorithm.DP;

/**
 * @brief:                  范围上尝试的模型
 *              给定一个整型数组arr，代表数值不同的纸牌排成一条线，
 *              玩家A和玩家B依次拿走每张纸牌，规定玩家A先拿，玩家B后拿，
 *              但是每个玩家每次只能拿走最左或最右的纸牌，
 *              玩家A和玩家B都绝顶聪明。请返回最后获胜者的分数
 * */
public class C18_02_CardsInLine {
    public static Integer tryWin(Integer[] arr) {
        if (arr == null || arr.length == 0) return 0;
        return Math.max(first(arr, 0, arr.length - 1), second(arr, 0, arr.length - 1));
    }

    // 先手获得的最大分数(假设我是先手)
    public static Integer first(Integer[] arr, Integer L, Integer R) {
        // 只剩下一张牌,直接拿走
        if (L == R) return arr[L];

        Integer s1 = arr[L] + second(arr, L + 1, R); // 拿走左侧牌
        Integer s2 = arr[R] + second(arr, L, R - 1); // 拿走右侧牌
        return Math.max(s1, s2);
    }

    // 后手获得的最大分数(假设我是后手)
    public static Integer second(Integer[] arr, Integer L, Integer R) {
        // 只剩下一张牌,只能被先手拿走
        if (L == R) return 0;

        Integer s1 = first(arr, L + 1, R);  // 先手拿走左侧牌
        Integer s2 = first(arr, L, R - 1);  // 先手拿走右侧牌
        return Math.min(s1, s2); // 先手让后手(我)拿到的分数最低
    }

    public static void main(String[] args) {
        // Case1 容错测试
        Integer[] arr0 = new Integer[0];
        assert (tryWin(arr0) == 0);

        Integer[] arr1 = new Integer[]{1, 100, 2};
        assert (tryWin(arr1) == 100);

        Integer[] arr2 = new Integer[]{1, 100, 2, 4};
        assert (tryWin(arr2) == 104);

        Integer[] arr3 = new Integer[]{2, 10, 11, 1};
        assert (tryWin(arr3) == 13);
    }
}

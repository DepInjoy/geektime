package algorithm.training.algorithm.recursion;

/**
 * @brief:      用递归方法求数组arr[L..R]中的最大值
 *
 *              实现思路:
 *                  1. 将[L..R]范围分成左右两半。左：[L..Mid]  右[Mid+1..R]
 *                  2. 左部分求最大值，右部分求最大值(是个递归过程，当范围上只剩下一个数便结束递归)
 *                  3. [L..R]范围上的最大值，是max{左部分最大值，右部分最大值}
 * */
public class 03_08_GetMax {
}

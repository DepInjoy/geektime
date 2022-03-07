package algorithm.training.algorithm.BitOperation;

/**
 *  @brief:     如何不用额外变量交换两个数
 *
 *              实现思想：主要利用异或的性质
 *                  1. a ^ a = 0
 *                  2. b ^ 0 = b
 * */
public class D02_01_Swap {
    public static void swap(int[] arr, int i, int j) {
        if (i == j) {
            System.out.println(i + " must not be equal to " + j);
            return;
        }

        arr[i] = arr[i] ^ arr[j];
        arr[j] = arr[i] ^ arr[j];
        arr[i] = arr[i] ^ arr[j];
    }

    public static void main(String[] args) {
        int[] arr = {1, 2, 3, 4};
        swap(arr, 0, 1);
        System.out.println(arr[0] + " " + arr[1]);
    }
}

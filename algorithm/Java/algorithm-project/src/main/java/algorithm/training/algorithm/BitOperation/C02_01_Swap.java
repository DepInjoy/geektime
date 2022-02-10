package algorithm.training.algorithm.BitOperation;

/**
 * @brief:      不用额外变量交换两个数，其中这两个数必须指向不同的地址空间
 *
 *              实现思路：
 *                  采用异或运算n,利用性质N^N=0
 * */
public class C02_01_Swap {
    public static void swap (int[] arr, int i, int j) {
        if (i == j) {
            System.out.println( i + " should not be equal " + j);
            return;
        }

        arr[i] = arr[i] ^ arr[j];
        arr[j] = arr[i] ^ arr[j];
        arr[i] = arr[i] ^ arr[j];
    }

    public static void main(String[] args) {
        int[] arr = new int[]{0, 1, 2, 3, 4, 5, 6, 7};
        swap(arr, 0, 1);
        System.out.println(arr[0] + " " + arr[1]);
        swap(arr, 2, 3);
        System.out.println(arr[2] + " " + arr[3]);
        swap(arr, 4, 5);
        System.out.println(arr[4] + " " + arr[5]);
        swap(arr, 6, 7);
        System.out.println(arr[6] + " " + arr[7]);
    }
}

package algorithm.training.algorithm.BinarySearch;

/**
 * @brief:      局部最小值问题,其中数组无序且任意位置不相等
 *              阶梯思路:
 *              .                 .
 *              |                 |
 *              |  |   ......   | |
 *              L  L+1        R-1 R
 *              如果存在这样的结构,那么在L-R之间有可能存在\/局部区域
 * */
public class C01_06_BSAwesome {
    public static int getLessIndex(int[] arr) {
        if (arr == null || arr.length == 0)
            return -1;

        if (arr.length == 1 || arr[0] < arr[1])
            return 0;

        if (arr[arr.length - 1] < arr[arr.length -2])
            return arr.length - 1;

        int L = 1, R = arr.length - 2;
        while (L <= R) {
            int mid = L + ((R - L) >> 1);
            if (arr[mid - 1] < arr[mid]) {
                R =  mid - 1;
            } else if (arr[mid] > arr[mid + 1]) {
                L = mid + 1;
            } else { // arr[mid-1] < arr[mid] < arr[mid+1]
                return mid;
            }
        }
        return L;
    }
}

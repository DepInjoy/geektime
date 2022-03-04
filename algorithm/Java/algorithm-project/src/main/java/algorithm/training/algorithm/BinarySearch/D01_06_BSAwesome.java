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
public class D01_06_BSAwesome {
    public static int getLessIndex(int[] sortedArr) {
        if (sortedArr == null || sortedArr.length < 1) return -1;
        if (sortedArr.length == 1 || sortedArr[0] < sortedArr[1]) return 0;
        if (sortedArr[sortedArr.length-2] > sortedArr[sortedArr.length - 1])
            return sortedArr.length - 1;

        int L = 0, R = sortedArr.length - 1;
        while (L <= R) {
            int mid = L + ((R - L) >> 1);
            //      \       /
            // mid-1 mid mid+1
            if (sortedArr[mid-1] > sortedArr[mid] && sortedArr[mid] < sortedArr[mid+1]) {
                return mid;
            } else if (sortedArr[mid] < sortedArr[mid+1]) { // L\L+1  mid/mid+1
                R = mid + 1;
            } else { // mid+1 \ mid  R-1/R
                L = mid - 1;
            }
        }
        return L;
    }

    public static void main(String[] args) {
        int[] arr = {0, -1}; // 1
        System.out.println(getLessIndex(arr));

        int[] arr0 = {0, 1}; // 1
        System.out.println(getLessIndex(arr0));

        int[] arr1 = {0, 1, 2, 3, 4}; // 0
        System.out.println(getLessIndex(arr1));

        int[] arr2 = {4, 3, 2, 1}; // 3
        System.out.println(getLessIndex(arr2));

        int[] arr3 = {1, 0, 2}; // 1
        System.out.println(getLessIndex(arr3));

        int[] arr4 = {1, 0, 2, 4, 3, 5, 7, 6, 8}; // 4符合
        System.out.println(getLessIndex(arr4));
    }
}

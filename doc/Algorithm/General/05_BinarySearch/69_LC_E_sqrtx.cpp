/**
 * 69. x 的平方根 
 * https://leetcode.cn/problems/sqrtx/description/
*/

int mySqrt(int x) {
    int l = 0, r = x;
    while (l < r) {
        long long mid = ((long long)r + l + 1) >> 1;
        if (mid <= x / mid) l = mid;
        else r = mid - 1;
    }
    return l;
}
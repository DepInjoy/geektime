/**
 * @brief   给你一个非负整数x ，计算并返回x的算术平方根
 *          由于返回类型是整数，结果只保留整数部分 ，小数部分将被舍去 
 * 
 */

int mySqrt(int x) {
   if (x < 0) return -1;

   int l = 0, r = x, mid = (l + r) >> 1;
   while (l < r) {
      mid = (l + r) >> 1;
      if (mid * mid > x) {
         r = mid - 1;
      } else if (mid * mid < x) {
         if((mid+1) * (mid+1) > x) return mid;
         l = mid;
      } else {
         return mid;
      }
   }
   return mid;
}
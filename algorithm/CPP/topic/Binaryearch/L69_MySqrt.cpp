/**
 * @brief   给你一个非负整数x ，计算并返回x的算术平方根
 *          由于返回类型是整数，结果只保留整数部分 ，小数部分将被舍去 
 *          链接：https://leetcode.cn/problems/sqrtx/
 */

int mySqrt(int x) {
   if (x < 0) return -1;

   int l = 0, r = x;
   int ans = 0;
   while (l <= r) {
      int mid = l + ((r - l) >> 1);
      // 避免2147395599数据溢出
      if ((long long)mid * mid <= x) {
        ans = mid;
        l = mid + 1;
      } else{
         r = mid - 1;
      }
   }
   return ans;
}
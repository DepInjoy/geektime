/**
 * 790. 数的三次方根
 * https://www.acwing.com/problem/content/792/
*/

#include <iostream>

int main() {
    const double allowed_error = 1e-8;
    double n;
    scanf("%lf", &n);
    
    double l = -10000, r = 10000;
    while (r - l > 1e-8) {
        double mid = (l + r)/2;
        if (mid * mid * mid > n) r = mid;
        else l = mid; 
    }
    // std::cout默认保留6位有效数字
    printf("%lf", l);

    return 0;
}
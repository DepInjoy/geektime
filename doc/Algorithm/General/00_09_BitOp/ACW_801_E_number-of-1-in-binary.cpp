/**
 * 801. 二进制中1的个数
 * https://www.acwing.com/problem/content/803/
*/

#include <iostream>

int lowbit(int n) {
   return n & (~n + 1); 
}

int main() {
    int n;
    scanf("%d", &n);
    
    while (n--) {
        int val;
        scanf("%d", &val);
        
        int ans = 0;
        while (val) val -= lowbit(val), ++ans;
        printf("%d ", ans);
    }
    return 0;
}
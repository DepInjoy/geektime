/**
 * 830. 单调栈
 * https://www.acwing.com/problem/content/832/
*/

#include <iostream>

const int N = 100010;
int stk[N], tt;

int main() {
    int n;
    scanf("%d", &n);

    while (n--) {
        int x;
        scanf("%d", &x);
    
        while (tt && stk[tt] >= x) tt--;
        if (tt) std::cout << stk[tt] << " ";
        else std::cout << "-1 ";
        stk[++tt] = x;
    }

    return 0;
}
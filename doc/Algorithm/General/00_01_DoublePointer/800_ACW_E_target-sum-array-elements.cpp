/**
 * 
*/

#include <iostream>

const int N = 100010;
int a[N], b[N];

int main() {
    int m, n, x;
    scanf("%d%d%d", &m, &n, &x);
    for (int i = 0; i < m; ++i) scanf("%d", &a[i]);
    for (int i = 0; i < n; ++i) scanf("%d", &b[i]);
    
    for (int i = 0, j = n - 1; i < m; ++i) {
        while(a[i] + b[j] > x) --j;
        if (a[i] + b[j] == x) {
            std::cout << i << " " << j;
        }
    }
    return 0;
}
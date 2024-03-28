/**
 * 125. 耍杂技的牛
 * https://www.acwing.com/problem/content/127/
*/

#include <climits>
#include <iostream>
#include <algorithm>

typedef std::pair<int, int> PII;
const int N = 50010;
PII d[N];

int main() {
    int n;
    scanf("%d", &n);
    for (int i = 0; i < n; ++i) {
        int w, s;
        scanf("%d%d", &w, &s);
        d[i] = {w+s, s}; 
    }
    
    std::sort(d, d+n);
    int ans = INT_MIN, sum = 0;
    for (int i = 0; i < n; ++i) {
        int s = d[i].second, w = d[i].first - s;
        ans = std::max(ans, sum - s);
        sum += w;
    }
    std::cout << ans << std::endl;
    
    return 0;
}

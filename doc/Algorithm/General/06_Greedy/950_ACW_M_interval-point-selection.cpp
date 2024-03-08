/**
 * 905. 区间选点
 * https://www.acwing.com/problem/content/description/907/
*/
#include <iostream>
#include <algorithm>
#include <limits.h>

const int N = 100010;

struct Range {
  int l, r;
  bool operator<(const Range& other) {
      return r < other.r;
  }
} ranges[N];

int main() {
    int n;
    scanf("%d", &n);
    
    for (int i = 0; i < n; ++i) {
        scanf("%d%d", &ranges[i].l, &ranges[i].r);
    }
    std::sort(ranges, ranges + n);
    
    int end = INT_MIN, ans = 0;
    for (int i = 0; i < n; ++i) {
        if (end < ranges[i].l) {
            end = ranges[i].r;
            ans++;
        }
    }
    std::cout << ans;
    return 0;
}
/**
 * 799.最长连续不重复子序列
 * https://www.acwing.com/problem/content/801/
*/

#include <iostream>

const int N = 100010;
int data[N], cnt[N];

int main() {
    int n;
    scanf("%d", &n);
    for (int i = 0; i < n; ++i) scanf("%d", &data[i]);

    int ans = 0;
    for (int i = 0, j = 0; i < n; ++i) {
        ++cnt[data[i]];
        while (cnt[data[i]] > 1) --cnt[data[j++]];
        ans = std::max(ans, i - j + 1);
    }
    std::cout << ans;
    return 0;
}
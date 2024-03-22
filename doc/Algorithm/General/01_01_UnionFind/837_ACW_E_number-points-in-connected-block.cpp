/**
 * 837. 连通块中点的数量
 * https://www.acwing.com/problem/content/839/
*/

#include <iostream>

const int N = 100010;
int d[N], cnt[N];

int find(int x) {
    if (d[x] != x) d[x] = find(d[x]);
    return d[x];
}

int main() {
    int n, m;
    scanf("%d%d", &n, &m);
    for (int i = 1; i <= n; ++i) {
        d[i] = i, cnt[i] = 1;
    }
    
    while (m--) {
        char op[3];
        scanf("%s", op);

        int a, b;
        if (op[0] == 'C') {
            scanf("%d%d", &a, &b);

            int pa = find(a), pb = find(b);
            if (pa == pb) continue;
            cnt[pb] += cnt[pa];
            d[pa] = pb;
        } else if (op[1] == '1') {
            scanf("%d%d", &a, &b);
            std::cout << (find(a) == find(b) ? "Yes" : "No") << std::endl;
        } else {
            scanf("%d", &a);
            std::cout << cnt[find(a)] << std::endl;
        }
    }

    return 0;
}

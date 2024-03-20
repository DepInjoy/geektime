/**
 * 835. Trie字符串统计
 * https://www.acwing.com/problem/content/description/837/
*/

#include <iostream>

// son[][]存储子节点的位置，题目指定输入字符串仅包含小写英文字母，因此分支最多26条
// cnt[]存储以某节点结尾的字符串个数（同时标记结尾）
// idx表示当前要插入的节点是第几个,每创建一个节点值+1
const int N = 100010;
int son[N][26], cnt[N], idx;
// 输入的字符串
char str[N];

void insert(char str[]) {
    int p = 0;
    for (int i = 0; str[i]; ++i) {
        int u = str[i] - 'a';
        // 节点不存在，创建节点
        if (!son[p][u]) son[p][u] = ++idx;
        p = son[p][u];
    }
    cnt[p]++;
}

int query(char str[]) {
    int p = 0;
    for (int i = 0; str[i]; ++i) {
        int u = str[i] - 'a';
        if (!son[p][u]) return 0;
        p = son[p][u];
    }
    return cnt[p];
}

int main() {
    int n;
    scanf("%d", &n);
    
    while (n--) {
        char op[2];
        scanf("%s%s", op, str);
        
        if (*op == 'I') insert(str);
        else std::cout << query(str) << std::endl;
    }
    
    return 0;
}

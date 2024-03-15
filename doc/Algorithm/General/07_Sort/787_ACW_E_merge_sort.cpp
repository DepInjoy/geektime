/**
 * 787. 归并排序
 * https://www.acwing.com/problem/content/789/
*/

#include <iostream>

const int N = 100010;
int d[N], tmp[N];

void merge_sort(int l, int r) {
    if (l >= r) return;
    
    int mid = l + r >> 1;
    merge_sort(l, mid), merge_sort(mid + 1, r);
    
    int i = l, j = mid + 1, k = 0;
    while (i <= mid && j <= r) {
        if (d[i] < d[j]) {
            tmp[k++] = d[i++];
        } else {
            tmp[k++] = d[j++];
        }
    }
    while(i <= mid) tmp[k++] = d[i++];
    while(j <= r) tmp[k++] = d[j++];
    
    for (int i = 0; i < k; ++i) d[l+i] = tmp[i];
}

int main() {
    int n;
    scanf("%d", &n);
    for (int i = 0; i < n; ++i) {
        scanf("%d", &d[i]);
    }
    
    merge_sort(0, n-1);

    for (int i = 0; i < n; ++i) {
        printf("%d ", tmp[i]);
    }
    return 0;
}
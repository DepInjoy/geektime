/**
 * 788. 逆序对的数量
 * https://www.acwing.com/problem/content/790/
*/

#include <iostream>

const int N = 100010;
int data[N], tmp[N];

long long merge_sort(int data[], int l ,int r) {
    if (l >= r) return 0;
    
    int mid = (l + r) >> 1;
    long long ans = merge_sort(data, l, mid) + merge_sort(data, mid + 1, r);
    
    int i = l, j = mid + 1, k = 0;
    while (i <= mid && j <= r) {
        if (data[i] <= data[j]) tmp[k++] = data[i++];
        else {
            tmp[k++] = data[j++];
            ans += mid - i + 1;
        }
    }
    while (i <= mid) tmp[k++] = data[i++];
    while (j <= r) tmp[k++] = data[j++];
    
    for (i = l, j = 0; i <= r; ++i, ++j) data[i] = tmp[j];
    return ans;
}

int main() {
    int n = 0;
    scanf("%d", &n);
    for (int i = 0; i < n; ++i) scanf("%d", &data[i]);
    
    std::cout << merge_sort(data, 0, n-1) << std::endl;
    return 0;
}
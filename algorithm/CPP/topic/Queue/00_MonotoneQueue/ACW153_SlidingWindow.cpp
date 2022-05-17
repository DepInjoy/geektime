/**
 * @brief   154. 滑动窗口
 *          给定一个大小为 n≤106 的数组。有一个大小为 k 的滑动窗口，它从数组的最左边移动到最右边。
 *          你只能在窗口中看到 k 个数字。每次滑动窗口向右移动一个位置。
 *          确定滑动窗口位于每个位置时，窗口中的最大值和最小值
 * 
 *          链接：https://www.acwing.com/problem/content/156/
 * 
 *          输入格式：  输入包含两行。
 *              第一行包含两个整数 n 和 k，分别代表数组长度和滑动窗口的长度。
 *              第二行有 n 个整数，代表数组的具体数值。同行数据之间用空格隔开。
 *          
 *          输出格式：输出包含两个。
 *              第一行输出，从左至右，每个位置滑动窗口中的最小值。 
 *              第二行输出，从左至右，每个位置滑动窗口中的最大值。
 *          
 *          输入样例：
 *              8 3
 *              1 3 -1 -3 5 3 6 7
 *          输出样例：
 *              -1 -3 -3 -3 3 3
 *              3 3 5 5 6 7
 */

#include <iostream>

using namespace std;

const int N = 1000010;
int a[N], q[N];

int main() {
    int n, k;
    cin >> n >> k;
    for (int i = 0; i < n; i++) cin >> a[i];
    
    int hh = 0, tt = -1;
    for (int i = 0; i < n; i++) {
        // 是否划出窗口
        if (hh <= tt && i - k + 1 > q[hh]) hh++;
        // 不符合单调性，弹出数据
        while(hh <= tt && a[q[tt]] >= a[i]) tt--;
        q[++tt] = i;
        
        if (i >= k-1) cout << a[q[hh]] << " ";
    }
    cout << endl;
    
    hh = 0, tt = -1;
    for (int i = 0; i < n; i++) {
        if (hh <= tt && i - k + 1 > q[hh]) hh++;
        while(hh <= tt && a[q[tt]] <= a[i]) tt--;
        q[++tt] = i;
        
        if (i >= k-1) cout << a[q[hh]] << " ";
    }
    return 0;
}
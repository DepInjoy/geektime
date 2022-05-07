/**
 * @brief  
 *          给定一个长度为 N 的整数数列，输出每个数左边第一个比它小的数，如果不存在则输出 −1。
 *          输入格式:
 *              第一行包含整数 N，表示数列长度. 第二行包含 N 个整数，表示整数数列。
 *          输出格式
 *              共一行，包含N个整数，其中第i个数表示第i个数的左边第一个比它小的数，如果不存在则输出 −1。
 *          数据范围:1≤N≤105, 1≤数列中元素≤109
 *          链接：https://www.acwing.com/problem/content/832/
 */

#include<iostream>
using namespace std;

const int N = 100000;
int stack[N], tt = 0;
int main() {
    int n;
    cin >> n;
    for (int i = 0; i < n; i++) {
        int x;
        cin >> x;
        while(tt && stack[tt] >= x) tt--;
        if (!tt) {
            cout << -1 << " ";
        } else{
            cout << stack[tt] << " ";
        }
        stack[++tt]=x;
    }
    return 0;
}

#if 0
#include<iostream>
#include<stack>
using namespace std;

const int N = 100000;
stack<int> stk;
int main() {
    int n;
    cin >> n;
    stk.push(-1);
    for (int i = 0; i < n; i++) {
        int x;
        cin >> x;
        while(stk.size() && stk.top() >= x) stk.pop();
        cout << stk.top() << " ";
        stk.push(x);
    }
    return 0;
}
#endif
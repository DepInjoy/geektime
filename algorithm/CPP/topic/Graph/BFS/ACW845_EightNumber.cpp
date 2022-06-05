/**
 * @brief   845. 八数码
 *          在一个 3×3 的网格中，1∼8 这 8 个数字和一个 x 恰好不重不漏地分布在这 3×3 的网格中。
 *          例如：
 *              1 2 3
 *              x 4 6
 *              7 5 8
 *          在游戏过程中，可以把 x 与其上、下、左、右四个方向之一的数字交换（如果存在）。
 *          我们的目的是通过交换，使得网格变为如下排列（称为正确排列）：
 *              1 2 3
 *              4 5 6
 *              7 8 x
 *          例如，示例中图形就可以通过让 x 先后与右、下、右三个方向的数字交换成功得到正确排列。
 *          交换过程如下：
 *              1 2 3   1 2 3   1 2 3   1 2 3
 *              x 4 6   4 x 6   4 5 6   4 5 6
 *              7 5 8   7 5 8   7 x 8   7 8 x
 *          现在，给你一个初始网格，请你求出得到正确排列至少需要进行多少次交换。
 * 
 *          输入格式：输入占一行，将 3×3 的初始网格描绘出来。
 *          例如，如果初始网格如下所示：
 *              1 2 3 
 *              x 4 6 
 *              7 5 8 
 *          则输入为：1 2 3 x 4 6 7 5 8
 *          
 *          输出格式：输出占一行，包含一个整数，表示最少交换次数。如果不存在解决方案，则输出 −1。
 * 
 *          链接：https://www.acwing.com/problem/content/847/
 * 
 */

#include <iostream>
#include <string>
#include <queue>
#include <unordered_map>

using namespace std;

int bfs(std::string start) {
    std::string end("12345678x");
    std::unordered_map<string, int> state;
    std::queue<string> q;
    
    q.push(start);
    state[start] = 0;
    while(!q.empty()) {
        auto t = q.front();
        q.pop();
        int distance = state[t];
        if(t == end) return distance;

        int pos = t.find("x");
        int dx[4] = {-1, 0, 1, 0}, dy[4] = {0, -1, 0, 1};
        for (int i = 0; i < 4; i++) {
            int x = pos / 3 + dx[i], y = pos % 3 + dy[i];
            if (x >= 0 && x < 3 && y >= 0 && y < 3) {
                swap(t[3*x+y], t[pos]);
                if (state.find(t) == state.end()) {
                    state[t] = distance + 1;;
                    q.push(t);
                }
                swap(t[3*x+y], t[pos]);
            }
        }
    }
    return -1;
}

int main() {
    std::string start;
    for (int i = 0; i < 9; i++) {
        char c[2];
        cin >> c;
        start.append(c);
    }
    
    std::cout << bfs(start) << std::endl;
    return 0;
}
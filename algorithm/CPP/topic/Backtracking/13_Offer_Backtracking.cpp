/**
 * 
 */

#include <string>
#include <vector>
using namespace std;

/**
 * @brief 剑指Offer-12:矩阵中的路径
 *          给定一个mxn二维字符网格board和一个字符串单词word。
 *          如果 word 存在于网格中，返回 true ；否则，返回 false 。
 *          单词必须按照字母顺序，通过相邻的单元格内的字母构成，其中“相邻”单元格是那些水平相邻或垂直相邻的单元格。
 *          同一个单元格内的字母不允许被重复使用。
 * 
 *          https://leetcode.cn/problems/ju-zhen-zhong-de-lu-jing-lcof/description
 */
bool check(const vector<vector<char>>& board, const string& word,
        vector<vector<bool>>& visited, const int m, const int n,
        const int i, int j, int k) {
    if (board[i][j] != word[k]) {
        return false;
    } else if(k == word.size() -1) {
        return true;
    }
    visited[i][j] = true;
    std::vector<std::pair<int, int>> directions{
        {0, 1}, {0, -1}, {-1, 0}, {1, 0}
    };
    bool ans = false;
    for (auto direction : directions) {
        int newi = i + direction.first, newj = j + direction.second;
        if (newi >=0 && newi < m && newj >= 0 && newj < n) {
            if (!visited[newi][newj]) {
                if (check(board, word, visited, m , n, newi, newj, k+1)) {
                    ans = true;
                }
            }
        }
    }
    visited[i][j] = false;
    return ans;
}

bool exist(vector<vector<char>>& board, string word) {
    int m = board.size(), n = board[0].size();
    std::vector<std::vector<bool>> visited(m, std::vector(n, false));
    for (int i = 0; i < m; ++i) {
        for (int j = 0; j < n; ++j) {
            if (check(board, word, visited, m, n, i, j, 0)) {
                return true;
            }
        }
    }
    return false;
}

/**
 * @brief   面试题13. 机器人的运动范围
 *              地上有一个m行n列的方格，从坐标 [0,0] 到坐标 [m-1,n-1] 。
 *              一个机器人从坐标 [0, 0] 的格子开始移动，它每次可以向左、右、上、下移动一格（不能移动到方格外），
 *              也不能进入行坐标和列坐标的数位之和大于k的格子。
 *              例如，当k为18时，机器人能够进入方格 [35, 37] ，因为3+5+3+7=18。
 *              但它不能进入方格 [35, 38]，因为3+5+3+8=19。
 *              请问该机器人能够到达多少个格子？
 * 
 *          https://leetcode.cn/problems/ji-qi-ren-de-yun-dong-fan-wei-lcof/description
 */

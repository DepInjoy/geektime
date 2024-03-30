/**
 *  165. 比较版本号
 *  https://leetcode.cn/problems/compare-version-numbers/description/)
 * 
 *  双指针算法
*/

#include <string>
using namespace std;

int compareVersion(string version1, string version2) {
    const int m = version1.size(), n = version2.size();
    int i = 0, j = 0;
    while (i < m || j < n) {
        long x = 0, y = 0;
        for (; i < m && version1[i] != '.'; ++i) {
            x = x * 10 + version1[i] - '0';
        }
        ++i;

        for (; j < n && version2[j] != '.'; ++j) {
            y = y * 10 + version2[j] - '0';
        }
        ++j;

        if (x != y) {
            return (x >  y) ? 1 : -1;
        }
    }

    return 0;
}
/**
 * @brief       给定一个有序数组arr，代表坐落在X轴上的点
                给定一个正数K，代表绳子的长度
                返回绳子最多压中几个点？(即使绳子边缘处盖住点也算盖住)

                实现思想：窗口实现
                    建立l和r两个index,当arr[r]-arr[l]<=绳子长度，则r++,否则l++
 * 
 */
#include <iostream>
#include <vector>

int maxPoint(std::vector<int>& arr, int L) {
    int l = 0, r = 0, N = arr.size();
    int maxPoints = 0;
    while (l < N) {
        while (r < N && arr[r] - arr[l] <= L) {
            r++;
        }
        maxPoints = std::max(maxPoints, r - (l++));
    }
    return maxPoints;
}

int main(int argc, char* argv[]) {
    std::vector<int> arr{1, 2, 3, 4, 5};
    int L = 3;
    std::cout << maxPoint(arr, L) << std::endl;
    return 0;
}

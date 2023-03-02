#include <vector>
#include <set>

using namespace std;

/**
 * @brief   剑指Offer-40:最小的k个数
 *              输入整数数组 arr ，找出其中最小的 k 个数。
 *              例如，输入4、5、1、6、2、7、3、8这8个数字，则最小的4个数字是1、2、3、4。
 * 
 *          https://leetcode.cn/problems/zui-xiao-de-kge-shu-lcof/description/
 */

// 利用堆
vector<int> getLeastNumbers(vector<int>& arr, int k) {
    std::multiset<int, std::greater<int>> mset;
    for (int item : arr) {
        if (mset.size() < k) {
            mset.insert(item);
        } else if (*mset.begin() > item) {
            mset.erase(mset.begin());
            mset.insert(item);
        }    
    }

    std::vector<int> ans(k);
    int i = 0;
    for (int d : mset) {
        ans[i++] = d;
    }
    return ans;
}


// 手动构建大顶堆
void down(vector<int>& arr, const int i, const int size) {
    int t = i;
    if (2*i+1 < size && arr[2*i+1] > arr[t]) t = 2*i+1;
    if (2*i+2 < size && arr[2*i+2] > arr[t]) t = 2*i+2;
    if (t != i) {
        std::swap(arr[t], arr[i]);
        down(arr, t, size);
    }
}

vector<int> getLeastNumbers(vector<int>& arr, int k) {
    if (k == 0) return{};

    std::vector<int> ans(k);
    for (int i = 0; i < k; ++i) {
        ans[i] = arr[i];
    }

    for (int i = k/2-1; i>=0; --i) {
        down(ans, i, k);
    }

    for (int i = k; i < arr.size(); ++i) {
        if (arr[i] < ans[0]) {
            ans[0] = arr[i];
            down(ans, 0, k);
        }
    }
    return ans;
}
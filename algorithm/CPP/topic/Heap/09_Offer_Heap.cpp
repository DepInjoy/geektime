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

/**
 *  剑指Offer专项 II 059. 数据流的第 K 大数值
 *  https://leetcode.cn/problems/jBjn9C/description/
 *  
 * 设计一个找到数据流中第 k 大元素的类（class）。注意是排序后的第 k 大元素，不是第 k 个不同的元素。
 *  请实现 KthLargest 类：
 *      KthLargest(int k, int[] nums) 使用整数 k 和整数流 nums 初始化对象。
 *      int add(int val) 将 val 插入数据流 nums 后，返回当前数据流中第 k 大的元素。
*/
class KthLargest {
public:
    KthLargest(int k, vector<int>& nums) {
        size = k;
        for (int num : nums) {
            add(num);
        }
    }
    
    int add(int val) {
        if (minHeap.size() < size) {
            minHeap.push(val);
        } else if (val > minHeap.top()) {
            minHeap.pop();
            minHeap.push(val);
        }
        return minHeap.top();
    }
private:
    uint32_t size{0};
    std::priority_queue<int, std::vector<int>, std::greater<int>> minHeap;
};

/**
 *  剑指 Offer II 060. 出现频率最高的 k 个数字(中等)
 *  https://leetcode.cn/problems/g5c51o/description/
 *
 *      给定一个整数数组 nums 和一个整数 k ，请返回其中出现频率前 k 高的元素。可以按 任意顺序 返回答案。
*/
vector<int> topKFrequent(vector<int>& nums, int k) {
    const int n = nums.size();
    std::unordered_map<int, int> numFreqMap(n);
    auto cmp = [](std::pair<int, int> l, std::pair<int, int> r) {
        return l.second > r.second;
    };
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int> >, decltype(cmp)> minHeap(cmp);
    for (int num : nums) {
        numFreqMap[num]++;
    }

    for (auto iter = numFreqMap.begin(); iter != numFreqMap.end(); ++iter) {
        if (minHeap.size() < k) {
            minHeap.push(std::make_pair<>(iter->first, iter->second));
        } else if (iter->second > minHeap.top().second) {
            minHeap.pop();
            minHeap.push(std::make_pair<>(iter->first, iter->second));
        }
    }

    std::vector<int> ans(k);
    int i = 0;
    while (!minHeap.empty()) {
        ans[i++] = minHeap.top().first;
        minHeap.pop();
    }
    return ans;
}

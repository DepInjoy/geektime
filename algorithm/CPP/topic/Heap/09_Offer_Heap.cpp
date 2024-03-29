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

/**
 * 基于堆的实现，时间复杂度为O(Nlogk), 空间复杂度为O(N)
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

/**
 *  基于快排的实现, 平均时间复杂度为O(N), 空间复杂度为O(N)
*/
void qsort(std::vector<std::pair<int, int>>& arr, int start,
        int end, std::vector<int>& topk, int k) {
    int picked = (std::rand() % (end - start + 1)) + start;
    std::swap(arr[picked], arr[end]);

    int pivot = arr[end].second;
    int index = start - 1;
    for(int j = start; j < end; ++j) {
        if (arr[j].second >= pivot) {
            std::swap(arr[++index], arr[j]);
        }
    }
    std::swap(arr[index + 1], arr[end]);

    if (start + k <= index + 1) {
        qsort(arr, start, index, topk, k);
    } else {
        for (int i = start; i <= index + 1; ++i) {
            topk.push_back(arr[i].first);
        }
        if (start + k > index + 2) {
            qsort(arr, index + 2, end, topk, k - (index + 2 - start));
        }
    }
}

vector<int> topKFrequent(vector<int>& nums, int k) {
    std::unordered_map<int, int> numFreqMap(nums.size());
    for (int num : nums) {
        numFreqMap[num]++;
    }

    std::vector<std::pair<int, int>> numFreqArr(numFreqMap.size());
    int i = 0;
    for (auto iter : numFreqMap) {
        numFreqArr[i++] = iter;
    }

    std::vector<int> ans;
    ans.reserve(k);
    qsort(numFreqArr, 0, numFreqArr.size() - 1, ans, k);
    return ans;
}

/**
 *  剑指 Offer II 061. 和最小的 k 个数对
 *  https://leetcode.cn/problems/qn8gGX/description
 * 
 *      给定两个以升序排列的整数数组 nums1 和 nums2 , 以及一个整数 k 。
 *      定义一对值 (u,v)，其中第一个元素来自 nums1，第二个元素来自 nums2 。
 *      请找到和最小的 k 个数对 (u1,v1),  (u2,v2)  ...  (uk,vk) 。
*/

/**
 *  基于大顶堆实现
*/
vector<vector<int>> kSmallestPairs(vector<int>& nums1, vector<int>& nums2, int k) {
    auto cmp = [](const std::pair<int, int>& l, const std::pair<int, int>& r) {
        return (l.first + l.second) < (r.first + r.second);
    };
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>,
            decltype(cmp)> maxHeap(cmp);
    const int n1 = nums1.size(), n2 = nums2.size();
    for (int i = 0; i < std::min(k, n1); ++i) {
        for(int j = 0; j < std::min(k, n2); ++j) {
            if (maxHeap.size() < k) {
                maxHeap.push(std::make_pair(nums1[i], nums2[j]));
            } else {
                std::pair<int, int> htop = maxHeap.top();
                if (nums1[i] + nums2[j] <= htop.first + htop.second) {
                    maxHeap.pop();
                    maxHeap.push(std::make_pair(nums1[i], nums2[j]));
                }
            }
        }
    }

    std::vector<std::vector<int>> ans;
    ans.reserve(k);
    while(!maxHeap.empty()) {
        std::pair<int, int> htop = maxHeap.top();
        ans.push_back({htop.first, htop.second});
        maxHeap.pop();
    }
    return ans;
}

/**
 * 基于小顶堆实现
*/
vector<vector<int>> kSmallestPairs(vector<int>& nums1, vector<int>& nums2, int k) {
    auto cmp = [&nums1, &nums2](const std::pair<int, int>& l, const std::pair<int, int>& r) {
        return nums1[l.first] + nums2[l.second] > nums1[r.first] + nums2[r.second];
    };
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>,
            decltype(cmp)> minHeap(cmp);
    const int n1 = nums1.size(), n2 = nums2.size();
    std::vector<std::vector<int>> ans;
    ans.reserve(k);
    for (int i = 0; i < std::min(k, n1); ++i) {
        minHeap.push(std::make_pair(i, 0));
    }

    while (k-- > 0 && !minHeap.empty()) {
        auto [i, j] = minHeap.top();
        ans.push_back({nums1[i], nums2[j]});
        minHeap.pop();
        if (j + 1 < n2) {
            minHeap.push(std::make_pair(i, j +1));
        }
    }
    return ans;
}
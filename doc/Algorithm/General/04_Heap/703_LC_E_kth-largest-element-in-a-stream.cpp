/**
 * 703. 数据流中的第 K 大元素
 * https://leetcode.cn/problems/kth-largest-element-in-a-stream/description/
 * https://leetcode.cn/problems/jBjn9C/description/
 */

class KthLargest {
public:
    KthLargest(int k, vector<int>& nums) : k_{k} {
        for (int num : nums) {
            add(num);
        }
    }
    
    int add(int val) {
        if (minheap.size() < k_) {
            minheap.push(val);
        } else if (val > minheap.top()) {
            minheap.pop();
            minheap.push(val);
        } 
        return minheap.top();
    }
private:
    int k_;
    std::priority_queue<int, std::vector<int>,
                std::greater<int>>    minheap;
};
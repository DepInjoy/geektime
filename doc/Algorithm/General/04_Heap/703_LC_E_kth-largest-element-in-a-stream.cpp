/**
 * 703. 数据流中的第 K 大元素
 * https://leetcode.cn/problems/kth-largest-element-in-a-stream/description/
*/

class KthLargest {
public:
    KthLargest(int k, vector<int>& nums) {
        this->k = k;
        for (int num : nums) {
            add(num);
        }
    }
    
    int add(int val) {
        heap.push(val);
        if (heap.size() > k) {
            heap.pop();
        }
        return heap.top();
    }
private:
    std::priority_queue<int, std::vector<int>, std::greater<int>> heap;
    int k;
};
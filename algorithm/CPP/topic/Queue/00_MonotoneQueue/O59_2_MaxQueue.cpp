/**
 * @brief   剑指 Offer 59 - II. 队列的最大值
 *          请定义一个队列并实现函数 max_value 得到队列里的最大值，
 *          要求函数max_value、push_back 和 pop_front 的均摊时间复杂度都是O(1)。
 *          若队列为空，pop_front 和 max_value 需要返回 -1）
 *          链接：https://leetcode.cn/problems/dui-lie-de-zui-da-zhi-lcof
 * 
 *          实现思想：
 *              1. 单调队列
 */

class MaxQueue {
public:
    MaxQueue() {}
    
    int max_value() {
        if (nums.empty()) return -1;
        return maxQ.front();
    }
    
    void push_back(int value) {
        nums.push(value);
        while(!maxQ.empty() && maxQ.back() <= nums.back()) maxQ.pop_back();
        maxQ.push_back(nums.back());
    }
    
    int pop_front() {
        if (nums.empty()) return -1;
        int ans = nums.front();
        if (!maxQ.empty() && maxQ.front() == ans) maxQ.pop_front();
        nums.pop();
        return ans;
    }

private:
    deque<int>      maxQ;
    queue<int>      nums;  
};

/**
 * @brief   155. 最小栈
 *          设计一个支持push, pop, top操作并能在常数时间内检索到最小元素的栈
 *          实现 MinStack 类:
 *              MinStack() 初始化堆栈对象
 *              void push(int val) 将元素val推入堆栈
 *              void pop() 删除堆栈顶部的元素
 *              int top() 获取堆栈顶部的元素
 *              int getMin() 获取堆栈中的最小元素
 *          
 *          链接：https://leetcode-cn.com/problems/min-stack
 */

class MinStack {
public:
    MinStack() {
    }
    
    void push(int val) {
        dataStack_.push(val);
        if (minStack_.empty() || val <= minStack_.top()) {
            minStack_.push(val);
        }
    }
    
    void pop() {
        int top = dataStack_.top();
        dataStack_.pop();
        if (!minStack_.empty() && top == minStack_.top()) {
            minStack_.pop();
        }
    }
    
    int top() {
        return dataStack_.top();
    }
    
    int getMin() {
        return minStack_.top();
    }
private:
    std::stack<int>     dataStack_;
    std::stack<int>     minStack_;
};

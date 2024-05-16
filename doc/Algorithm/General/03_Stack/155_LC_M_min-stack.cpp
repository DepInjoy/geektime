/**
 * 155. 最小栈
 * https://leetcode.cn/problems/min-stack/
*/

// 可以节约空间
class MinStack {
public:
    MinStack() {
        min_stk.push(INT_MAX);
    }
    
    void push(int val) {
        data.push(val);
        if (val <= min_stk.top()) {
            min_stk.push(val);
        }
    }
    
    void pop() {
        if (data.top() == min_stk.top()) {
            min_stk.pop();
        }
        data.pop();
    }
    
    int top() {
        return data.top();
    }
    
    int getMin() {
        return min_stk.top();
    }
private:
    std::stack<int> data;
    std::stack<int> min_stk;
};

// 实现方式2: 每次将将要push的元素和单调栈顶最小值push到单调栈
class MinStack {
public:
    MinStack() {
        min_stk.push(INT_MAX);
    }
    
    void push(int val) {
        data.push(val);
        min_stk.push(std::min(val, min_stk.top()));
    }
    
    void pop() {
        data.pop();
        min_stk.pop();
    }
    
    int top() {
        return data.top();
    }
    
    int getMin() {
        return min_stk.top();
    }
private:
    std::stack<int> data;
    std::stack<int> min_stk;
};
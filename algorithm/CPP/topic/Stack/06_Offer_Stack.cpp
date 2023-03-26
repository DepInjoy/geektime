
#include <stack>

/**
 * @brief  剑指Offer-09：用两个栈实现队列
 *              用两个栈实现一个队列。实现它的两个函数appendTail和deleteHead，
 *              分别完成在队列尾部插入整数和在队列头部删除整数的功能。(若队列中没有元素，deleteHead 操作返回-1)
 * 
 *         https://leetcode.cn/problems/yong-liang-ge-zhan-shi-xian-dui-lie-lcof/description
 */

class CQueue {
public:
    CQueue() {
    }
    
    void appendTail(int value) {
        s1_.push(value);
    }
    
    int deleteHead() {
        if (s2_.empty()) {
            while (!s1_.empty()) {
                s2_.push(s1_.top());
                s1_.pop();
            }
        }

        if (s2_.empty()) {
            return -1;
        }
        int val = s2_.top();
        s2_.pop();
        return val;
    }

private:
    std::stack<int>     s1_;
    std::stack<int>     s2_;
};

/**
 * @brief  剑指Offer-30：包含min函数的栈
 *              定义栈的数据结构，请在该类型中实现一个能够得到栈的最小元素的min函数在该栈中
 *              调用 min、push 及 pop 的时间复杂度都是 O(1)。
 * 
 *         https://leetcode.cn/problems/bao-han-minhan-shu-de-zhan-lcof/description
 */

class MinStack {
public:
    MinStack() {
        min_.push(INT_MAX);
    }
    
    void push(int x) {
        data_.push(x);
        min_.push(std::min(min_.top(), x));
    }
    
    void pop() {
        min_.pop();
        data_.pop();
    }
    
    int top() {
        return data_.top();
    }
    
    int min() {
        return min_.top();
    }

private:
    std::stack<int> data_;
    std::stack<int> min_;
};

// 减少一点内存占用
class MinStack2 {
public:
    MinStack() {
        min_.push(INT_MAX);
    }
    
    void push(int x) {
        data_.push(x);
        if (x <= min_.top()) {
            min_.push(x);
        }
    }
    
    void pop() {
        if (data_.top() == min_.top()) {
            min_.pop();
        }
        data_.pop();
    }
    
    int top() {
        return data_.top();
    }
    
    int min() {
        return min_.top();
    }

private:
    std::stack<int> data_;
    std::stack<int> min_;
};

/**
*   剑指 Offer II 039. 直方图最大矩形面积
*       给定非负整数数组 heights ，数组中的数字用来表示柱状图中各个柱子的高度。
        每个柱子彼此相邻，且宽度为1 。求在该柱状图中，能够勾勒出来的矩形的最大面积

    https://leetcode.cn/problems/0ynMMM/description
*/

// 暴力求解，时间复杂度O(N^2)，空间复杂度O(1)
int largestRectangleArea(vector<int>& heights) {
    int maxArea = 0;
    for (int i = 0; i < heights.size(); ++i) {
        int height = heights[i];
        for (int j = i; j < heights.size(); ++j) {
            height = std::min(height, heights[j]);
            int width = j - i + 1;
            maxArea = std::max(maxArea, height * width);
        }
    }
    return maxArea;
}

// 单调栈

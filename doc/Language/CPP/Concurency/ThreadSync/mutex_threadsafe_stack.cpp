/**
 *  实现线程安全的栈，对外提供了两个pop接口，
 *      1. 接受引用参数，该引用指向外部变量地址，存储弹出的值
 *      2. 返回std::shared_ptr<>智能指针，储存弹出的值
 *
 *  编译指令:
 *   g++ -pthread -std=c++11 -o mutex_threadsafe_stack mutex_threadsafe_stack.cpp
 * 
*/
#include <memory>
#include <stack>
#include <mutex>
#include <thread>
#include <exception>
#include <iostream>

class StackEmpty : std::exception {
public:
    StackEmpty(const std::string& errorMsg = "oh no, stack is empty!") :
        errorMsg_(errorMsg) { }
    
    StackEmpty(const StackEmpty& other) {
        errorMsg_ = other.errorMsg_;
    }

    const char* what() const throw() {
        return errorMsg_.c_str();
    }
private:
    std::string     errorMsg_;
};

template<typename T>
class ThreadSafeStack {
public:
    ThreadSafeStack() {}
    ThreadSafeStack& operator=(const ThreadSafeStack&) = delete;

    ThreadSafeStack(const ThreadSafeStack& other) {
        std::lock_guard<std::mutex> l(other.lock);
        data = other.data;
    }

    void push(const T& value) {
        std::lock_guard<std::mutex> l(lock);
        data.push(value);
    }

    std::shared_ptr<T> pop() {
        std::lock_guard<std::mutex> l(lock);
        if (data.empty()) {
            throw StackEmpty();
        }

        std::shared_ptr<T> ans = std::make_shared<T>(data.top());
        data.pop();
        return ans;
    }

    void pop(T& value) {
        std::lock_guard<std::mutex> l(lock);
        if (data.empty()) {
            throw StackEmpty();
        }

        value = data.top();
        data.pop();
    }

    bool empty() {
        std::lock_guard<std::mutex> l(lock);
        return data.empty();
    }
private:
    std::stack<T>   data;
    std::mutex      lock;
};

ThreadSafeStack<int> gstack;

void ProduceTask() {
    int value = 0;
    while (value <= 10) {
        gstack.push(++value);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void ConsumeTask() {
    while(true) {
        try {
            // 在step 1时判断了栈不空
            // 在step 2时可能中间调用了pop接口，导致此时栈为空而抛出异常
            if (!gstack.empty()) {  // step 1
                int curVal = -1;
                gstack.pop(curVal); // step 2
                std::cout << "Consume thread " << std::this_thread::get_id()
                    << " get( " << curVal << " )" << std::endl;
            }
        } catch (StackEmpty& e) {
            //std::cout << " meet stack empty exception " << e.what() << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    const uint8_t  consumeTaskCnt = 5;
    std::thread* threads[consumeTaskCnt];
    for (int i = 0; i < consumeTaskCnt; ++i) {
        threads[i] = new std::thread(ConsumeTask);
    }
    std::thread produceThread(ProduceTask);

    for (int i = 0; i < consumeTaskCnt; ++i) {
        threads[i]->join();
        delete threads[i];
    }
    produceThread.join();
    return 0;
}
#include <memory>
#include <stack>
#include <mutex>
#include <thread>
#include <exception>
#include <iostream>

class StackEmpty : std::exception {
public:
    StackEmpty(const std::string& errorMsg = "oh no, stack is empty!") :
        errorMsg_(errorMsg) {}
    StackEmpty
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
    while (value <= 100) {
        gstack.push(++value);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void ConsumeTask() {
    while(true) {
        try {
            if (!gstack.empty()) {
                int curVal = -1;
                gstack.pop(curVal);
                std::cout << "Consume thread " << std::this_thread::get_id()
                    << " get( " << curVal << " )" << std::endl;
            }
        } catch (StackEmpty& e) {
            //std::cout << " meet stack empty exception " << e.what() << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    // 生成5个
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
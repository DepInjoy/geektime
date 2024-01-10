#include <atomic>
#include <thread>
#include <functional>
#include <iostream>
#include <vector>
#include "join_threads.h"
#include "thread_safe_queue.h"

class SimpleThreadPool {
public:
    SimpleThreadPool() : done_(false), joiner_(threads_){
        const unsigned int threadCount = std::thread::hardware_concurrency();
        try {
            for (int i = 0; i < threadCount; ++i) {
                threads_.push_back(std::thread(&SimpleThreadPool::doWork, this));
            }
        } catch(...) {
            done_ = true;
            throw;
        }
    }

    ~SimpleThreadPool() {
        done_ = true;
    }

    template<typename FunctionType>
    void submit(const FunctionType& task) {
        workers_.push(TaskFunctionType(task));
    }
private:
    void doWork() {
        while (!done_) {
            if (!workers_.empty()) {
                std::cout << " do work " << std::endl;
                TaskFunctionType task;
                workers_.try_pop(task);
                task();
            } else {
                // 当前线程出让时间片,等待其他线程添加任务
                std::this_thread::yield();
            }
        }
    }
private:
    // 假定任务没有返回值
    using TaskFunctionType = std::function<void()>;

    // 工作线程
    std::vector<std::thread>                threads_;
    // 使用线程安全队列管理线程
    ThreadSafeQueue<TaskFunctionType>       workers_;
    std::atomic_bool                        done_;
    // 确保启动的线程妥善终止(线程实例全部join)
    JoinThreads                             joiner_;
};

int main(int argc, char* argv[]) {
    auto showMessage = []() {
        std::cout << " Yes, You are here!";
    };

    SimpleThreadPool* simpleThreadPool = new SimpleThreadPool();
    for (int i = 0; i < 10; ++i) {
        std::cout << " submit task " << i << std::endl;
        simpleThreadPool->submit<std::function<void()> >(showMessage);
    }
    delete simpleThreadPool;
    getchar();
    return 0;
}
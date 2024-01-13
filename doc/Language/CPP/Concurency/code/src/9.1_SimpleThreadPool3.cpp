/**
 * 功能描述:
 *     采用了thread_local变量，令每个线程都具有自己的任务队列(local_worker_queue_)
 *     线程池本身则再维护一全局队列(global_worker_queue_)
 * 
 * 编译命令：
 *     g++ -o 9.1_SimpleThreadPool3 9.1_SimpleThreadPool3.cpp -pthread
*/
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <type_traits>
#include <vector>
#include <queue>
#include <memory>
#include <numeric>
#include <iostream>

template<typename T>
class ThreadSafeQueue {
public:
    bool empty() {
        std::lock_guard<std::mutex> lk(lock_);
        return datas_.empty();
    }

    void push(T&& item) {
        std::lock_guard<std::mutex> lk(lock_);
        datas_.push(std::move(item));
    }

    bool try_pop(T& item) {
        std::lock_guard<std::mutex> lk(lock_);
        if (datas_.empty()) {
            return false;
        }

        item = std::move(datas_.front());
        datas_.pop();
        return true;
    }
private:
    std::queue<T>               datas_;
    std::mutex                  lock_;
};

class JoinThreads {
public:
    JoinThreads(std::vector<std::thread>& threads) : threads_(threads) {}
    ~JoinThreads() {
        for (int i = 0; i < threads_.size(); ++i) {
            if (threads_[i].joinable()) {
                threads_[i].join();
            }
        }
    }
private:
    std::vector<std::thread>&   threads_;
};

class FunctionWrapper {
public:
    template<typename F>
    FunctionWrapper(F&& f) : impl_(new ImpleType<F>(std::move(f))) {
    }

    FunctionWrapper() = default;
    FunctionWrapper(FunctionWrapper&& other) {
        impl_ = std::move(other.impl_);
    }

    FunctionWrapper& operator=(FunctionWrapper&& other) {
        impl_ = std::move(other.impl_);
        return *this;
    }

    FunctionWrapper(const FunctionWrapper&) = delete;
    FunctionWrapper(FunctionWrapper&) = delete;
    FunctionWrapper& operator=(const FunctionWrapper&) = delete;

    void operator()() {
        impl_->run();
    }
private:
    class ImpleBase {
    public:
        virtual void run() = 0;
        ~ImpleBase() = default;
    };

    template<typename F>
    class ImpleType : public ImpleBase {
    public:
        ImpleType(F&& func) : func_(std::move(func)) {}
        void run() {
            func_();
        }
        F       func_;
    };
private:
    std::unique_ptr<ImpleBase>      impl_;
};

class SimpleThreadPool3 {
public:
    SimpleThreadPool3() : joiner_(threads_), done_(false) {
        const unsigned int cnt = std::thread::hardware_concurrency();
        try {
            for (int i = 0; i < cnt; ++i) {
                threads_.push_back(std::thread(&SimpleThreadPool3::doWork, this));
            }
        } catch (...) {
            done_ = true;
        }
    }

    template<typename FunctionType>
    std::future<typename std::result_of<FunctionType()>::type>
    submit(FunctionType func) {
        using result_type = typename std::result_of<FunctionType()>::type;
        std::packaged_task<result_type()> task(func);
        std::future<result_type> ans = task.get_future();
        if (local_worker_queue_) {
            std::cout << "submit task to local worker queue" << std::endl;
            local_worker_queue_->push(std::move(task));
        } else {
            std::cout << "submit task to global worker queue" << std::endl;
            global_worker_queue_.push(std::move(task));
        }
        return ans;
    }

    void shutdown() {
        done_ = true;
    }

private:
    void doWork() {
        local_worker_queue_.reset(new std::queue<FunctionWrapper>);
        while (!done_) {
            FunctionWrapper task;
            if (local_worker_queue_ && !local_worker_queue_->empty()) {
                task = std::move(local_worker_queue_->front());
                local_worker_queue_->pop();
                task();
            } else if (global_worker_queue_.try_pop(task)) {
                task();
            } else {
                std::this_thread::yield();
            }
        }
    }

public:
    std::vector<std::thread>                        threads_;
    JoinThreads                                     joiner_;
    std::atomic_bool                                done_;
    static thread_local std::unique_ptr<
        std::queue<FunctionWrapper> >                local_worker_queue_;
    ThreadSafeQueue<FunctionWrapper>                global_worker_queue_;
};

thread_local std::unique_ptr<std::queue<FunctionWrapper>>
    SimpleThreadPool3::local_worker_queue_ = nullptr;

template<typename Iterator, typename T>
struct AccumulateBlock {
    T operator()(Iterator begin, Iterator end) {
        return std::accumulate(begin, end, T());
    }
};

template<typename Iterator, typename T>
T ParallelAccumulate(Iterator begin, Iterator end, T init) {
    unsigned long const length = std::distance(begin, end);
    if (!length) return init;

    unsigned long const block_size = 25;
    unsigned long const block_num = (length + block_size - 1) / block_size;
    
    SimpleThreadPool3 thread_pool;
    Iterator start = begin;
    std::vector<std::future<T>> futures(block_num);
    for (int i = 0; i < block_num - 1; ++i) {
        futures[i] = thread_pool.submit([=]{
            return AccumulateBlock<Iterator, T>()(start, start + block_size);
        });
        std::advance(start, block_size);
    }

    futures[block_num - 1] = thread_pool.submit([=] {
        return AccumulateBlock<Iterator, T>()(start, end);
    });

    T ans = init;
    for (int i = 0; i < block_num; ++i) {
        ans += futures[i].get();
    }

    // 确保这个函数退出
    thread_pool.shutdown();
    return ans;
}

int main(int argc, char* argv[]) {
    std::vector<int> datas { 1, 2, 3, 4, 5, 6 };
    std::cout << "comming, and expect sum = " <<
        std::accumulate(datas.begin(), datas.end(), 0) << std::endl;
    int sum = ParallelAccumulate(datas.begin(), datas.end(), 0);
    std::cout << "Parallel sum = " << sum << std::endl;

    return 0;
}
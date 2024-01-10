/**
 *  g++ -o 9.1_SimpleThreadPool2 9.1_SimpleThreadPool2.cpp -pthread
*/
#include <future>
#include <thread>
#include <atomic>
#include <vector>
#include <functional>
#include <type_traits>
#include <numeric>
#include <iostream>

#include <memory> 
#include <deque>
#include <mutex>
#include <condition_variable>

template <typename T>
class ThreadSafeQueue {
public:
    ThreadSafeQueue& operator=(ThreadSafeQueue&) = delete;
    ThreadSafeQueue(const ThreadSafeQueue& other) {
        // 锁定两个互斥
        std::lock(mutex_, other.mutex_);
        // std::adopt_lock对象，以指明互斥已上锁
        // std::lock_guard实例接收锁的归属权,不在构造函数内试图另行加锁
        std::lock_guard<std::mutex> lk1(mutex_, std::adopt_lock);
        std::lock_guard<std::mutex> lk2(other.mutex_, std::adopt_lock);
        datas_ = other.datas_;
    }

    void push(const T& item) {
        std::lock_guard<std::mutex> lk(mutex_);
        datas_.push_back(item);
        cond_.notify_one();
    }

    bool try_pop(T& item) {
        std::lock_guard<std::mutex> lk(mutex_);
        if (datas_.empty()) {
            return false;
        }

        item = std::move(datas_.front());
        datas_.pop_front();
    }

    std::shared_ptr<T> try_pop() {
        std::shared_ptr<std::mutex> lk(mutex_);
        if (datas_.empty) {
            return std::shared_ptr<T>();
        }

        std::shared_ptr<T> ans = std::make_shared<T>(datas_.front());
        datas_.pop_front();
        return ans;
    }

    void wait_and_pop(T& item) {
        std::unique_lock<std::mutex> lk(mutex_);
        cond_.wait(lk, [this]{
            return !datas_.empty();
        });

        item = datas_.front();
        datas_.pop();
    }

    std::shared_ptr<T> wait_and_pop() {
        std::unique_lock<std::mutex> lk(mutex_);
        cond_.wait(lk, [this]{
            return !datas_.empty();
        });

        std::shared_ptr<T> ans = std::make_shared<T>(datas_.front());
        datas_.pop();
        return ans;
    }

    bool empty() const {
        std::lock_guard<std::mutex> lk(mutex_);
        return datas_.empty();
    }
private:
    std::deque<T>               datas_;
    std::mutex                  mutex_;
    std::condition_variable     cond_;
};

class JoinThreads {
public:
    explicit JoinThreads(std::vector<std::thread>& threads) :
        threads_(threads) {}
    
    ~JoinThreads() {
        for (int i = 0; i < threads_.size(); ++i) {
            if (threads_[i].joinable()) {
                threads_[i].join();
            }
        }
    }
private:
    std::vector<std::thread>&       threads_;
};

class FunctionWrapper {
public:
    template<typename F>
    FunctionWrapper(F&& f) : impl_(new ImpleType<F>(f)){}
    void operator()(void) {
        impl_->Call();
    }

    FunctionWrapper(FunctionWrapper&& other) :
        impl_(std::move(other.impl_)) {}
    FunctionWrapper& operator=(FunctionWrapper&& other) {
        impl_ = std::move(other.impl_);
        return *this;
    }

    FunctionWrapper(const FunctionWrapper&) = delete;
    FunctionWrapper(FunctionWrapper&) = delete;
    FunctionWrapper& operator=(const FunctionWrapper&) = delete;
private:
    struct ImpleBase {
        virtual void Call() = 0;
        virtual ~ImpleBase() = default;
    };

    template <typename FunctionType>
    struct ImpleType : ImpleBase {
        FunctionType func;
        ImpleType(FunctionType&& f) : func(std::move(f)) {}
        void Call() {
            func();
        }
    };
private:
    std::unique_ptr<ImpleBase>      impl_;
};

class SimpleThreadPool2 {
public:
    SimpleThreadPool2() : done_(false), joiner_(workers_) {
        const unsigned int thread_cnt = std::thread::hardware_concurrency();
        try {
            for (int i = 0; i < thread_cnt; ++i) {
                workers_.push_back(std::this_thread(&SimpleThreadPool2::doWork, this));
            }
        } catch (...) {
            done_ = true;
            throw;
        }
    }

    template<typename FunctionType>
    std::future<std::result_of<FunctionType()>::type> submit(FunctionType func) {
        typedef typename std::result_of<FunctionType()>::type result_type;
        std::packaged_task<result_type()> task(std::move(func));
        std::future<result_type> ans(task.get_future());
        queue_.push(std::move(task));

        return ans;
    }
private:
    void doWork() {
        while (!done_) {
            FunctionWrapper task;
            if (queue_.try_pop(task)) {
                task();
            } else {
                std::this_thread::yield();
            }
        }
    }
private:
    std::atomic_bool                    done_;
    std::vector<std::thread>            workers_;
    JoinThreads                         joiner_;
    ThreadSafeQueue<FunctionWrapper>    queue_;
};


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
    
    SimpleThreadPool2 thread_pool;
    Iterator start = begin;
    std::vector<std::future<T>> futures(block_num);
    for (int i = 0; i < block_num - 1; ++i) {
        futures[i] = thread_pool.submit([=] {
            return AccumulateBlock<Iterator, T>()(start, start + block_size);
        });
        start = start + block_size;
    }
    futures[block_num - 1] = thread_pool.submit([=] {
            return AccumulateBlock<Iterator, T>()(start, end);
        });

    T ans = init;
    for (int i = 0; i < block_num; ++i) {
        ans += futures[i].get();
    }

    return ans;
}

int main(int argc, char* argv[]) {
    std::vector<int> datas{1, 2, 3, 4, 5, 6};
    int sum = ParallelAccumulate(datas.begin(), datas.end(), 0);
    std::cout << "sum is " << sum << std::endl;
    return 0;
}
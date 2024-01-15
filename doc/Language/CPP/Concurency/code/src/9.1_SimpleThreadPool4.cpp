/**
 * 功能实现：
 *     每个线程配备独立的任务队列，各自线程尝试从本地队列取任务(local_work_queue_)
 *     若本地队列没有任务，尝试从全局队列(global_work_queue_)取任务
 *     若全局队列和线程自身的局部队列都没有任务，便从其他的队列窃取任务
 *
 * 编译命令：
 *  g++ -o 9.1_SimpleThreadPool4 9.1_SimpleThreadPool4.cpp -pthread
*/
#include <thread>
#include <future>
#include <mutex>
#include <condition_variable>
#include <type_traits>
#include <vector>
#include <deque>
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
        datas_.push_back(std::move(item));
    }

    bool try_pop(T& item) {
        std::lock_guard<std::mutex> lk(lock_);
        if (datas_.empty()) {
            return false;
        }

        item = std::move(datas_.front());
        datas_.pop_front();
        return true;
    }
private:
    std::deque<T>               datas_;
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

/**
 * 可以改进为无锁队列,实现在本地线程在一端压入和弹出任务,其他线程在另一端窃取任务
 * 
 * @描述：
 *      这里采用互斥锁保护数据，假设窃取任务很少发生，互斥上很少发生任务争抢
 *      
 *      对于本地线程，任务后进先出进，是栈模式
 *      从缓存的视角，有助于性能改进，原因是对比早前的任务向队列压入的数据
 *      与现行任务相关的数据更有可能还留在缓存内。
 *      这很好地契合了快速排序这一类算法。
 *      
 */
class WorkStealQueue {
public:
    using T = FunctionWrapper;

    WorkStealQueue() = default;
    WorkStealQueue(const WorkStealQueue&) = delete;
    WorkStealQueue& operator=(const WorkStealQueue&) = delete;

    bool empty() const {
        std::lock_guard<std::mutex> lk(lock_);
        return queue_.empty();
    }

    void push(T&& item) {
        std::lock_guard<std::mutex> lk(lock_);
        queue_.push_front(std::move(item));
    }

    bool try_pop(T& item) {
        std::lock_guard<std::mutex> lk(lock_);
        if (queue_.empty()) {
            return false;
        }

        item = std::move(queue_.front());
        queue_.pop_front();
        return true;
    }

    bool try_steal(T& item) {
        std::lock_guard<std::mutex> lk(lock_);
        if (queue_.empty()) {
            return false;
        }

        item = std::move(queue_.back());
        queue_.pop_back();
        return true;
    }    
private:
    mutable std::mutex          lock_;
    std::deque<T>       queue_;
};

class SimpleThreadPool4 {
public:
    SimpleThreadPool4() : joiner_(worker_threads_), done_(false) {
        const unsigned int cnt = std::thread::hardware_concurrency();
        try {
            worker_threads_.reserve(cnt);
            worker_queues_.reserve(cnt);
            for (int i = 0; i < cnt; ++i) {
                worker_queues_.push_back(std::unique_ptr<WorkStealQueue>(new WorkStealQueue));
                worker_threads_.push_back(std::thread(&SimpleThreadPool4::doWork, this, i));
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
        if (local_work_queue_) {
            std::cout << "submit task to local worker queue" << std::endl;
            local_work_queue_->push(std::move(task));
        } else {
            std::cout << "submit task to global worker queue" << std::endl;
            global_work_queue_.push(std::move(task));
        }
        return ans;
    }

    void shutdown() {
        done_ = true;
    }
private:
    void doWork(int index) {
        index_ = index;
        local_work_queue_ = worker_queues_[index].get();
        while (!done_) {
            runPendingTask();
        }
    }

    void runPendingTask() {
        FunctionWrapper task;
        if (local_work_queue_->try_pop(task)) {
            task();
        } else if (global_work_queue_.try_pop(task)) {
            task();
        } else {
            const int n = worker_queues_.size();
            unsigned int i = 0;
            for (; i < n; ++i) {
                if (worker_queues_[(i + index_) % n]->try_pop(task)) {
                    task();
                    break;
                }
            }

            // work steal fail
            if (i == n) {
                std::this_thread::yield();
            }
        }
    }
private:
    std::vector<std::thread>                        worker_threads_;
    JoinThreads                                     joiner_;
    std::atomic_bool                                done_;
    ThreadSafeQueue<FunctionWrapper>                global_work_queue_;
    std::vector<std::unique_ptr<WorkStealQueue>>    worker_queues_;
    static thread_local WorkStealQueue*             local_work_queue_;
    static thread_local int index_;
};

thread_local int SimpleThreadPool4::index_ = 0;
thread_local WorkStealQueue* SimpleThreadPool4::local_work_queue_ = nullptr;


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

    unsigned long const block_size = 2;
    unsigned long const block_num = (length + block_size - 1) / block_size;
    
    SimpleThreadPool4 thread_pool;
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
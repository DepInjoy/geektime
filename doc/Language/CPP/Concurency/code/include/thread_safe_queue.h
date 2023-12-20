#ifndef __THREAD_SAFE_QUEUE_H__
#define __THREAD_SAFE_QUEUE_H__
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
        datas_.push(item);
        cond_.notify_one();
    }

    bool try_pop(T& item) {
        std::lock_guard<std::mutex> lk(mutex_);
        if (datas_.empty()) {
            return false;
        }

        item = datas_.front();
        datas_.pop();
    }

    std::shared_ptr<T> try_pop() {
        std::shared_ptr<std::mutex> lk(mutex_);
        if (datas_.empty) {
            return std::shared_ptr<T>();
        }

        std::shared_ptr<T> ans = std::make_shared<T>(datas_.front());
        datas_.pop();
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
#endif
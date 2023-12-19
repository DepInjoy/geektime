#ifndef __THREAD_SAFE_QUEUE_H__
#define __THREAD_SAFE_QUEUE_H__
#include <queue>
#include <mutex>

template <typename Type>
class ThreadSafeQueue {
public:
    void push(const Type& item) {
        datas_.push(item);
    }

    void try_pop(Type& item) {
        item = datas_.front();
        datas_.pop();
    }

    bool empty() const {
        return datas_.empty();
    }
private:
    std::queue<Type>    datas_;
    std::mutex          mutex_;
};
#endif
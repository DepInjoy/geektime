#ifndef __JOIN_THREADS_H__
#define __JOIN_THREADS_H__
#include <vector>
#include <thread>

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
#endif
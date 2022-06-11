#include <iostream>
#include <string>
#include <chrono>

class TimeCost {
public:
    explicit TimeCost(std::string uniqueFlag) : uniqueFlag_(uniqueFlag) {
        startPoint_ = std::chrono::system_clock::now();
    }

    uint64_t getIntervalMS() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now() - startPoint_).count();
    }

    ~TimeCost() {
        uint64_t elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::system_clock::now() - startPoint_).count();
        std::cout << uniqueFlag_ << " cost\t : " << elapsed_ms << std::endl;;
    }

private:
    std::chrono::time_point<std::chrono::system_clock>  startPoint_;
    std::string                                         uniqueFlag_;
};

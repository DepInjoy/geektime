#include <thread>
#include <future>
#include <iostream>
 
void do_work(std::promise<void>& barrier) {
    std::cout << "do work [IN]" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));
    barrier.set_value();
    std::cout << "do work [OUT]" << std::endl;
}


int main(int argc, char* argv[]) {
    std::promise<void> barrier;
    std::future<void> barrier_future = barrier.get_future();

    std::cout << "comming, barrier ......" << std::endl;
    std::thread worker(do_work, std::ref(barrier));
    barrier_future.wait();
    // 必须完成do_work()才会到这里
    std::cout << "barrier out ......" << std::endl;
    worker.join();
    return 0;
}
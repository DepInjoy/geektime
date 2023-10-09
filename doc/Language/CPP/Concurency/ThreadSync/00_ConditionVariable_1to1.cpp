/**
 * 编译指令
 *  g++ -pthread -std=c++11 00_ConditionVariable_1to1.cpp -o 00_ConditionVariable_1to1
*/
#include <deque>
#include <thread>
#include <mutex>
#include <iostream>
#include <condition_variable>

std::mutex lock;
std::condition_variable conVar;
std::deque<int> q;

void Producer() {
    int value = 0;
    while (value <= 10) {
        {
            std::unique_lock<std::mutex> wl(lock);
            q.push_front(value++);
        }
        std::cout << "Produce value = " << value << std::endl;
        conVar.notify_one();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void Consumer() {
    int consumeValue = 0;
    while (consumeValue < 10) {
        std::unique_lock<std::mutex> wl(lock);
        // 防止虚假唤醒
        while(q.empty()) {
            conVar.wait(wl);
        }
        consumeValue = q.back();
        q.pop_back();
        wl.unlock();
        std::cout << "Consumer Thread " << std::this_thread::get_id()
            << " get " << consumeValue << std::endl;
    }
    std::cout << "Consumer [OUT]" << std::endl;
}

int main(int argc, char* argv[]) {
    std::thread t1(Producer);
    std::thread t2(Consumer);
    t1.join();
    t2.join();
    getchar();
    return 0;
}

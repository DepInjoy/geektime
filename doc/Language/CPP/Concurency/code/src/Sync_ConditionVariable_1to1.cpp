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
        // 这里只能使用std::unique_lock不能使用std::lock_gard
        // 因为线程在wait(等待)期间，必须解锁互斥
        // 唤醒之后，需要重新上锁，std::lock_grad无法提供这样的灵活性
        std::unique_lock<std::mutex> wl(lock);
        // 防止虚假唤醒(两种实现等价)
#if 1
        while(q.empty()) {
            conVar.wait(wl);
        }
#else
        // 判断条件(lambda函数的返回值)
        // 如果返回true, 则wait返回
        // 如果返回false，则解锁互斥并令线程进入阻塞(或等待)状态
        // 待数据在其他线程准备好，调用notify_one()通知条件变量
        // 则该线程从休眠中唤醒(解除阻塞),重新在互斥上锁,再次检验条件
        // 如果条件成立，则wait返回，而且互斥仍被锁住
        // 如果条件不成立,则该线程解锁互斥，继续等待
        conVar.wait(wl, [](){
            return !q.empty();
        })
#endif
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

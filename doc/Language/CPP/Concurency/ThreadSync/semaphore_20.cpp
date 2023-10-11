/**
 * 编译指令:
 *      g++ -pthread -std=c++20 -o semaphore_20 semaphore_20.cpp
*/
#include <list>
#include <thread>
#include <mutex>
#include <memory>
#include <errno.h>
#include <iostream>
#include <semaphore>

class Task;
using TaskPtrType = std::shared_ptr<Task>;

std::list<TaskPtrType> tasks;
std::mutex locker;
std::counting_semaphore sem(0);

class Task {
public:
    Task(const int taskId) : taskId_(taskId) {}
    void doTask() {
        std::cout << "Consume thread " << std::this_thread::get_id()
            << " do task with id( " << taskId_ << " )" << std::endl;
    }
private:
    int     taskId_;
};

void ProduceTask() {
    int value = 0;
    while (true) {
        std::unique_lock<std::mutex> l(locker);
        tasks.push_back(std::make_shared<Task>(++value));
        l.unlock();
        sem.release();
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

void ConsumeTask() {
    while(true) {
        sem.acquire();
        std::unique_lock<std::mutex> l(locker);
        if (tasks.empty()) {
            continue;
        }
        tasks.front()->doTask();
        tasks.pop_back();
        l.unlock();
    }
}

int main(int argc, char* argv[]) {
    // 生成5个消费者
    const uint8_t  consumeTaskCnt = 5;
    std::thread* threads[consumeTaskCnt];
    for (int i = 0; i < consumeTaskCnt; ++i) {
        threads[i] = new std::thread(ConsumeTask);
    }
    // 生成一个消费者
    std::thread produceThread(ProduceTask);

    for (int i = 0; i < consumeTaskCnt; ++i) {
        threads[i]->join();
        delete threads[i];
    }
    produceThread.join();
    return 0;
}
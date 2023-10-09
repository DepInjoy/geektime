/**
 * 编译指令
 *  g++ -pthread -std=c++11 00_ConditionVariable_1toMore.cpp -o 00_ConditionVariable_1toMore
*/
#include <list>
#include <thread>
#include <mutex>
#include <iostream>
#include <condition_variable>

class Task;
using TaskPtrType = std::shared_ptr<Task>;

std::list<TaskPtrType> tasks;
std::mutex locker;
std::condition_variable conVar;


class Task {
public:
    Task(int taskId) : taskId_(taskId) {}
    void doTask() {
        std::cout << "Consume thread " << std::this_thread::get_id()
            << " do task with id( " << taskId_ << " )" << std::endl;
    }
private:
    int taskId_;
};

void ProduceTask() {
    int taskId = 0;
    while (true) {
        std::unique_lock<std::mutex> l(locker);
        tasks.push_back(std::make_shared<Task>(++taskId));
        l.unlock();
        conVar.notify_one();
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void ConsumeTask() {
    while (true) {
        std::unique_lock<std::mutex> l(locker);
        while (tasks.empty()) {
            conVar.wait(l);
        }
        tasks.front()->doTask();
        tasks.pop_front();
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

    // 生成1个生产者
    std::thread produceThread(ProduceTask);

    for (int i = 0; i < consumeTaskCnt; ++i) {
        threads[i]->join();
        delete threads[i];
    }
    produceThread.join();

    return 0;
}
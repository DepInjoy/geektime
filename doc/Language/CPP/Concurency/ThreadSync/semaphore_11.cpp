/**
 * 编译指令:
 *      g++ -pthread -std=c++11 -o semaphore_11 semaphore_11.cpp
*/
#include <list>
#include <thread>
#include <mutex>
#include <memory>
#include <errno.h>
#include <iostream>
#include <semaphore.h>

class Task;
using TaskPtrType = std::shared_ptr<Task>;

std::list<TaskPtrType> tasks;
std::mutex locker;

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

class Semaphore {
public:
    Semaphore(const unsigned int value = 0) {
        sem_init(&sem_, 0, value);
    }

    ~Semaphore() {
        sem_destroy(&sem_); // 销毁信号量
    }

    void wait() {
        while(sem_wait(&sem_) != 0);
    }

    bool try_wait() {
        if (sem_trywait(&sem_) != 0) {
            if (errno == EAGAIN) {
                return false;
            }
        }
        return true;
    }

    bool time_wait(const uint32_t sec) {
        struct timespec t;
        t.tv_sec = time(NULL) + sec;
        t.tv_nsec = 0;
        while (sem_timedwait(&sem_, &t) != 0) {
            if (errno == ETIMEDOUT) {
                std::cout << "semaphore wait timeout !" << std::endl; 
                return false;
            }
        }
        return true;
    }

    void post() {
        sem_post(&sem_);
    }
private:
    sem_t   sem_;
};

Semaphore sem;

void ProduceTask() {
    int value = 0;
    while (true) {
        std::unique_lock<std::mutex> l(locker);
        tasks.push_back(std::make_shared<Task>(++value));
        l.unlock();
        sem.post();
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }
}

void ConsumeTask() {
    while(true) {
        sem.wait();
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

    // 生成1个消费者
    std::thread produceThread(ProduceTask);

    for (int i = 0; i < consumeTaskCnt; ++i) {
        threads[i]->join();
        delete threads[i];
    }
    produceThread.join();
    return 0;
}
/**
 * 编译命令:
 *      g++ -o 4.2_OneOff_SimplePackagedTask 4.2_OneOff_SimplePackagedTask.cpp -pthread -std=c++11
*/
#include <future>
#include <thread>
#include <atomic>
#include <deque>
#include <sstream>
#include <iostream>

std::deque<std::packaged_task<void()>> tasks;
std::mutex lock;
std::atomic<bool> shutdown{false};

bool gui_shutdown_message_received() {
    return shutdown;
}

void get_and_process_gui_message() {
    // std::cout << "mock, get and process GUI message, in thread("
    //     << std::this_thread::get_id() << ")" << std::endl;
}

void gui_thread(void) {
    while (!gui_shutdown_message_received()) {
        get_and_process_gui_message();
        
        std::packaged_task<void()> task;
        {
            std::lock_guard<std::mutex> lk(lock);
            if (tasks.empty()) {
                continue;
            }

            task = std::move(tasks.front());
            tasks.pop_front();
        }
        // 执行任务
        task();
    }
}

template<typename Func>
std::future<void> post_task_for_gui_thread(Func f) {
    std::packaged_task<void()> task(f);
    std::future<void> ans = task.get_future();

    std::lock_guard<std::mutex> lk(lock);
    tasks.push_back(std::move(task));

    return ans;
}

int main(int argc, char* argv[]) {
    // 模拟生成一个GUI任务
    static int number = 0;
    auto gui_task = []() -> void {
        std::cout << "take show message as a gui task in "
            << number << std::endl;
    };

    // 运行GUI线程，实现取任务并执行
    std::thread th(gui_thread);

    do {
        // 提交任务(输入一个字符提交一个任务)
        post_task_for_gui_thread<void()>(gui_task);
        ++number;
    } while (getchar() != 'q');

    std::cout << "User input q, now quit" << std::endl;

    shutdown = true;
    th.join();
    return 0;
}
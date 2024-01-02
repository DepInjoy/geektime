/**
 * 编译命令：
 *      g++ -o 4.2_OneOff_SimpleSync 4.2_OneOff_SimpleSync.cpp -pthread -std=c++11
*/
#include <future>
#include <thread>
#include <sstream>
#include <iostream>

int find_answer(void) {
    std::cout << "Thread(" << std::this_thread::get_id()
        << ") call " << __FUNCTION__ << std::endl;
    // 等待1秒之后再返回
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return 0;
}

void do_other_stuff() {
    std::cout << "Thread(" << std::this_thread::get_id()
        << ") call " << __FUNCTION__ << std::endl;
}

struct X {
    void foo(int number, const std::string& msg) {
        std::cout << "Thread(" << std::this_thread::get_id()
            << ") call " << __FUNCTION__ << " number = " << number
            << " say " << msg << std::endl;
    }

    std::string bar(const std::string& msg) {
        std::ostringstream ss;
        ss << "Thread( " << std::this_thread::get_id() << ") call "
            << __FUNCTION__ << " say " << msg;
        return ss.str();
    }
};


int main(int argc, char* argv[]) {
    /**
     *  1. 后台运行方式,可以指定policy来指定运行方式,也可以不指定让其自行决定
     * 
     *  policy用于指定运行方式
     *      std::launch::async
     *          表示另外开启专属线程调用任务函数(该示例中,线程号和main函数线程号不一致)
     *      std::launch::deferred
     *          表示在当前线程延迟调用任务函数(该示例中,线程号和main函数线程号一致)
     *      std::launch::deferred | std::launch::async
     *          表示自行决定开启专属线程或延迟调用
     * */
    std::launch policy = std::launch::async;
    // std::launch policy = std::launch::deferred;
    std::future<int> answer = std::async(policy, find_answer);
    // 自行决定开启新线程或当前线程延迟调用
    // std::future<int> answer = std::async(find_answer);
    do_other_stuff();
    std::this_thread::sleep_for(std::chrono::seconds(5));

    int ans = answer.get();
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "main thread(" << std::this_thread::get_id()
        << ") got answer " << ans << std::endl;

    /**
     *  2. 给后台任务传递参数
    */
    X x;
    std::future<void> task1 = std::async(std::launch::async,
        &X::foo, &x, 0, "hello");
    std::this_thread::sleep_for(std::chrono::seconds(5));
    task1.get();

    std::future<std::string> task2 = std::async(
            &X::bar, &x, "goodbye");
    std::cout << task2.get() << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(5));

    return 0;
}
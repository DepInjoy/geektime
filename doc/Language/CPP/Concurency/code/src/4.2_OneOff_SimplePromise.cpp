/**
 * 模拟customer线程表示顾客等待奶茶制作
 * waiter表示服务员制作奶茶，当奶茶制作完成通知顾客取咖啡
 * 
 * 编译命令：
 *       g++ -o 4.2_OneOff_SimplePromise 4.2_OneOff_SimplePromise.cpp -pthread
*/

/**
 * 
 * 1 frame为partition
 *   select sum(c1) over  (partition by c1 order by c4) from test_table 
 * 
 * 2. frame为rows
 *   select sum(c1) over  (partition by c1 order by c4 rows between 5 preceding and current row) from test_table 
 * 
 * 3. frame为range
 *   select sum(c1) over  (partition by c1 order by c4 range between 5 preceding and 10 following) from test_table 

*/
#include <thread>
#include <future>
#include <functional>
#include <iostream>

void MakeMilkTea(std::promise<int>& event) {
    event.set_value(1);
    std::cout << "您的奶茶已经做好了" << std::endl;
}

void WaitForMilkTea(std::future<int>& fevent) {
    std::cout << "等待中..." << std::endl;
    int val = fevent.get();
    std::cout << "收到通知，马上去取奶茶" << std::endl;
}

int main(int argc, char* argv[]) {
    std::promise<int> event;
    std::future<int> fevent = event.get_future();

    std::thread customer(WaitForMilkTea, std::ref(fevent));
    std::this_thread::sleep_for(std::chrono::seconds(5));
    std::thread waiter(MakeMilkTea, std::ref(event));

    customer.join();
    waiter.join();
    return 0;
}
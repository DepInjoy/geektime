# 使用future等待一次性事件发生

使`std::future`和任务关联并非唯一的方法：

- 运用类模板`std::packaged_task<>`的实例，也能将任务包装起来
- 利用`std::promise<>`类模板编写代码，显式地异步求值



## 任务包装:关联future实例和任务







`std::packaged_task<>`具备函数调用操作符，`std::packaged_task<>`对象是可调用对象，可以直接使用，还可以包装在`std::function`对象中，当作线程函数传递给`std::thread`对象，也可以传递给需要可调用对象的函数。

定义特化的`std::packaged_task<>`类模板

```C++
// 书本只给出了部分代码，模板不会呢
template<>
class packaged_task<std::string(std::vector<char>*,int)> {
public:
    template<typename Callable>
    explicit packaged_task(Callable&& f);
    std::future<std::string> get_future();
    void operator()(std::vector<char>*,int);
};
```


有些任务无法以简单的函数调用表达出来，还有一些任务的执行结果可能来自多个部分的代码。如何处理？这种情况就需创建`future`：借助`std::promise`显式地异步求值。

## 创建std::promise显式地异步求值

>假设，有个应用需要处理大量网络连接，我们往往倾向于运用多个独立线程，一对一地处理各个连接，这能简化网络通信的构思，程序编写也相对容易。如果连接数量较少（因而线程数量也少），此方式行之有效；随着连接数量攀升，过多线程导致消耗巨量系统资源，一旦线程数量超出硬件所支持的并发任务数量，还可能引起繁重的上下文切换，影响性能。极端情况下，在网络连接超出负荷之前，操作系统就可能已经先耗尽别的资源，无法再运行新线程。故此，若应用要处理大量网络连接，通常交由少量线程负责处理（可能只有一个），每个线程同时处理多个连接。



例如，单个线程处理多个连接，采用一对`std::promise<bool>/std::future<bool>`，以确证数据包成功向外发送；与future关联的值是一个表示成败的简单标志。对于传入的数据包，与future关联的数据则是包内的有效荷载（payload)

```C++
#include <future>

void process_connections(connection_set& connections) {
    while (!done(connections)) {
        // 依次检查各个连接，如果有数据传入则接收;如果有数据发出，则向外发送数据
        for(connection_iterator connection = connections.begin(), end = connections.end();
                connection != end; ++connection) {
            // 有新数据传入
            if (connection->has_incoming_data()) {
                data_packet data = connection->incoming();
                // 定传入的数据包本身已含有ID和荷载数据
                // 令每个ID与各std::promise对象(可能存储到关联容器)一一对应
                std::promise<payload_type>& p = connection->get_promise(data.id);
                p.set_value(data.payload);
            }

            // 发送已入队的传出数据
            if (connection->has_outgoing_data()) {
                outgoing_packet data = connection->top_of_outgoing_queue();
                connection->send(data.payload);
                // 发送完成,将和数据发送相关的promise设置为true
                data.promise.set_value(true);
            }
        }
    }
}
```



上面所有的处理都未考虑异常，而线程在运行过程中可能会初选异常。如果为了采用`std::packaged_task`和`std::promise`，而强令保障所有代码都无异常，也不太现实。C++标准库给出了一种干净利落的方法，以在这种情形下处理异常，并且异常更能够被保存为相关结果的组成部分。


`std::future`只容许一个线程等待结果。若要让多个线程等待同一个目标事件，需要用`std::shared_future`。


# 限时等待

有两种超时(`timeout`)机制可供选用：一是迟延超时(`duration-based timeout`)，线程根据指定的时长而继续等待（如30毫秒）；二是绝对超时(`absolute timeout`)。




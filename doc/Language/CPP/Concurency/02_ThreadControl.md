
# 运行时选择线程数量
C++标准库的`std::thread::hardware_concurrency()`函数，它的返回值是一个指标，表示程序在各次运行中可真正并发的线程数量。硬件支持的线程数量有限，运行的线程数量不应超出该限度（超出的情况称为线程过饱和，即oversubscription），因为线程越多，上下文切换越频繁，导致性能降低

# 识别线程

线程ID所属型别是`std::thread::id`，函数定义位于头文件`<thread>`内。有两种获取方法：
1. 在与线程关联的`std::thread`对象上调用成员函数`get_id()`，即可得到该线程的ID。如果`std::thread`对象没有关联任何执行线程，调用`get_id()`则会返回一个`std::thread::id`对象，它按默认构造方式生成，表示“线程不存在”。
2. 当前线程的ID可以通过调用`std::this_thread::get_id()`获得。

## 数据操作
C++标准库提供的对线程ID的操作有
- 完整的比较运算，比较运算符对所有不相等的值确立了全序(Total Order)关系。全序关系是关于集合的数学概念，满足3个性质：反对称、传递和完全。这使得它们可以用作关联容器的键值，还可以用于排序，还可以参与任何用途的比较运算。
    ```C++
    operator ==
    
    // folowing removed in C++20
    operator !=
    operator <
    operator <=
    operator >
    operator >=

    // C++20,C++20采用<=>来比较两个参数的大小
    // 返回值采用std::strong_ordering表示,可能取值:
    //  std::strong_ordering::less （lhs less than rhs）
    //  std::strong_ordering::greater（otherwise rhs less than lhs）
    //  std::strong_ordering::equal (otherwise)
    operator<=>
    ```
- 标准库的hash模板计算Hash值，因此`std::thread::id`的值也可以用作新标准的无序关联容器。
    ```C++
    std::hash<std::thread::id>
    ```
- 支持将线程ID输出到输出流。确切的输出内容严格依赖于实现，C++标准只是保证了比较运算中相等的线程ID应产生一样的输出，而不相等的线程ID则产生不同的输出。主要用途是调试除错和记录日志。
    ```C++
    operator<<
    ```
## 常用场景
 1. **std::thread::id实例常用于识别线程，以判断它是否需要执行某项操作。**。例如,在运行线程中用线程切分任务，主线程负责发起其他线程且需要完成一些特殊的工作。
    ```C++
    std::thread::id master_thread;
    void some_core_part_of_algorithm() {
        // 保存主线程ID，确保可以识别并完成主线程需要完成的特殊操作
        if(std::this_thread::get_id() == master_thread) {
            // 主线程相对于其他任务进行一些特殊操作
            do_master_thread_work();
        }

        do_common_work();
    }
    ```
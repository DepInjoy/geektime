
# 运行时选择线程数量
C++标准库的`std::thread::hardware_concurrency()`函数，它的返回值是一个指标，表示程序在各次运行中可真正并发的线程数量。硬件支持的线程数量有限，运行的线程数量不应超出该限度（超出的情况称为线程过饱和，即oversubscription），因为线程越多，上下文切换越频繁，导致性能降低

# 识别线程

线程ID所属型别是`std::thread::id`，函数定义位于头文件`<thread>`内。有两种获取方法。
- 在与线程关联的`std::thread`对象上调用成员函数`get_id()`，即可得到该线程的ID。如果`std::thread`对象没有关联任何执行线程，调用`get_id()`则会返回一个`std::thread::id`对象，它按默认构造方式生成，表示“线程不存在”。
- 当前线程的ID可以通过调用`std::this_thread::get_id()`获得。


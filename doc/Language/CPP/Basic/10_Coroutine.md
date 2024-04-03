协程是一种可以被挂起和恢复的函数，它提供了一种创建异步代码的方法。C++20标准规定，出现`co_await`、`co_return`和`co_yield`3个关键字中任意一个的函数就是协程。

1. 如何让函数挂起和恢复呢？
2. 协程实现原理



# 实现原理

## co_await运算符原理



目标对象可被等待需要实现`await_resume`、`await_ready`和`await_suspen`3个成员函数，具备这3个函数的对象可以称为等待器，也就是说等待器和可等待体可以是同一个对象。那么等待器是做什么的，为什么要给同一个对象两种命名呢？我们需要从以上的3个函数开始讨论：

1. `await_ready`函数叫作`is_ready`或许更加容易理解，该函数用于判定可等待体是否已经准备好，也就是说可等待体是否已经完成了目标任务，如果已经完成，则返回true；否则返回false。
2. `await_suspend`这个函数名则更加令人难以理解，命名为`schedule_ continuation`应该会更加清晰，它的作用就是调度协程的执行流程，比如异步等待可等待体的结果、恢复协程以及将执行的控制权返回调用者。
3. `await_resume`实际上用于接收异步执行结果，可以叫作`retrieve_value`。



```C++
// 
awaitable_string operator co_await(std::string&& str) {
  return awaitable_string{ str };
}

// 
std::future<std::string> foo() {
  auto str = co_await std::string{ "hello" };
  co_return str;
}
```



## co_yield运算符原理



## co_return运算符原理


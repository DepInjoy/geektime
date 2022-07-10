#include <iostream>

void OutOfMemory() {
    std::cerr << "Unable to satisfy request for memory" << std::endl;
    // 移除new-handler
    std::set_new_handler(nullptr);
}

template<typename T>
class NewHandlerSupport {
public:
    virtual ~NewHandlerSupport() {
        // 恢复new-handler
        std::set_new_handler(curNewHandler_);
    }
    static std::new_handler set_new_handler(std::new_handler handler) throw();
    void* operator new(std::size_t size);

private:
    // 记录当前的new-handler
    static std::new_handler    curNewHandler_;
};

template<typename T>
std::new_handler NewHandlerSupport<T>::curNewHandler_ = nullptr;

template<typename T>
std::new_handler NewHandlerSupport<T>::set_new_handler(
        std::new_handler handler) throw() {
    std::new_handler oldHanler = curNewHandler_;
    curNewHandler_ = handler;
    return oldHanler;
}

template<typename T>
void* NewHandlerSupport<T>::operator new(std::size_t size) {
    NewHandlerSupport<T>::set_new_handler(std::set_new_handler(curNewHandler_));
    return ::operator new(size);
}

class Widget : public NewHandlerSupport<Widget> {
public:
    static void OutOfMemory(); // 自定义Widget内存分配失败的行为
};

void Widget::OutOfMemory() {
    std::cerr << "Alloc Memory for Widget failed!" << std::endl;
    // 其他行为

    // maybe now, 移除new-handler
    set_new_handler(nullptr);
}

int main(int argc, char* argv[]) {
    // 设置全局的new-handler
    std::set_new_handler(OutOfMemory);

    // 自定义Widget的new-handler行为
    Widget::set_new_handler(Widget::OutOfMemory);
    Widget* widget = new Widget();
    // 通过NewHandlerSupport的析构恢复全局的new-handler
    delete widget;

    try {
        const uint64_t allocSize = 100000000000L;
        int* pBigDataArray = new int[allocSize];
        delete [] pBigDataArray;
    } catch (std::bad_alloc& e) {
        std::cerr << "Alloc failed! " << e.what() << std::endl;
    }
    return 0;
}
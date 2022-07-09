#include <iostream>

void OutOfMemory() {
    std::cerr << "Unable to satisfy request for memory" << std::endl;
    // 移除new-handler
    std::set_new_handler(nullptr);
}

class NewHandlerHolder {
public:
    explicit NewHandlerHolder(const std::new_handler handler)
        : newHandler_(handler) {};
    ~NewHandlerHolder() {
        // 恢复new-handler
        std::set_new_handler(newHandler_);
    }
private:
    // 记录当前的new-handler
    std::new_handler    newHandler_;

private:
    // 禁止复制和拷贝
    NewHandlerHolder(const NewHandlerHolder&);
    NewHandlerHolder& operator =(const NewHandlerHolder&);
};

class Widget {
public:
    static void OutOfMemory(); // 自定义Widget内存分配失败的行为
    static std::new_handler set_new_handler(std::new_handler handler) throw();
    void* operator new(std::size_t size);

private:
    static std::new_handler currentNewHandler;
};
std::new_handler Widget::currentNewHandler = nullptr;

void Widget::OutOfMemory() {
    std::cerr << "Alloc Memory for Widget failed!" << std::endl;
    // 其他行为
}

std::new_handler Widget::set_new_handler(std::new_handler handler) throw() {
    std::new_handler oldNewHandler = currentNewHandler;
    currentNewHandler = handler;
    return oldNewHandler;
}

void* Widget::operator new(std::size_t size) {
    NewHandlerHolder holder(std::set_new_handler(currentNewHandler));
    return ::operator new(size);
}

int main(int argc, char* argv[]) {
    // 设置全局的new-handler
    std::set_new_handler(OutOfMemory);

    // 自定义Widget的new-handler行为
    Widget::set_new_handler(Widget::OutOfMemory);
    Widget* widget = new Widget();
    // 通过NewHandlerHolder的析构恢复全局的new-handler
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
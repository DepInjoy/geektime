/**
 * @brief   条款11: 禁止异常(exceptions)流出destructors之外
 *          
 *          全力保证异常不传出析构函数, 有两个理由
 *          1.避免terminate函数在exception传播过程的栈展开(stack-unwinding)机制中被调用
 *          2.协助确保destructors 完成其应该完成的所有事情
 */
#include <iostream>
#include <memory>
#include <exception>

class Session {
public:
    Session() {
        logCreation(this);
        StartTransaction();  // 开始事务
    }

    ~Session() {
        logDestruction(this);
        EndTransaction();  // 结束事务
    }

private:
    static void logCreation(Session* objAddr) {
        std::cout << "Session logCreation" << std::endl;
    }

    static void logDestruction(Session* objAddr) {
        throw std::runtime_error("just for test");
        std::cout << "Session logDestruction" << std::endl;
    }

    void StartTransaction() {
        std::cout << "Session StartTransaction" << std::endl;
    }

    void EndTransaction() {
        std::cout << "Session EndTransaction" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    try {
        std::unique_ptr<Session> sessionPtr(new Session);
    } catch(std::exception& e) {
        std::cout << "catch exception: " << e.what() << std::endl;
    }
    std::cout << "after exception, say fine" << std::endl;
    return 0;
}

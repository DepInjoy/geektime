/**
 * 编译指令:
 *     g++ -o value_category 00_01_value_category.cpp -std=c++11
*/

#include <iostream>

class BigMemoryPool {
public:
    static const int PoolSize = 4096;
    BigMemoryPool() : pool_(new char[PoolSize]) {}
    ~BigMemoryPool() {
        if (pool_ != nullptr) {
            delete[] pool_;
        }
    }

    BigMemoryPool(BigMemoryPool&& other) {
        std::cout << "move big memory pool." << std::endl;
        pool_ = other.pool_;
        other.pool_ = nullptr;
    }

    BigMemoryPool& operator=(BigMemoryPool&& other) {
        std::cout << "move(operator=) big memory pool." << std::endl;
        if (pool_ != nullptr) {
            delete[] pool_;
        }
        pool_ = other.pool_;
        other.pool_ = nullptr;
        return *this;
    }

    BigMemoryPool(const BigMemoryPool& other) :
            pool_(new char[PoolSize]) {
        std::cout << "copy big memory pool." << std::endl;
        memcpy(pool_, other.pool_, PoolSize);
    }

private:
    char *pool_;
};

BigMemoryPool get_pool(const BigMemoryPool& pool) {
    return pool;
}

BigMemoryPool make_pool() {
    BigMemoryPool pool;
    return get_pool(pool);
}

void move_pool1(BigMemoryPool &&pool) {
    std::cout << "call move_pool" << std::endl;
    // 无论函数的实参是左值还是右值，其形参都是一个左值
    // 即使这个形参看上去是一个右值引用
    // 这里采用复制构造函数对my_pool构造
    BigMemoryPool my_pool(pool);
}

void move_pool2(BigMemoryPool &&pool) {
    std::cout << "call move_pool" << std::endl;
    // 采用std::move将左值转换为右值
    // 使得采用移动构造函数对my_pool构造
    BigMemoryPool my_pool(std::move(pool));
}

int main() {
    move_pool1(make_pool());

    std::cout << std::endl
        << "make_pool2 use std::move convert lvalue to rvalue" << std::endl;
    move_pool2(make_pool());
    return 0;
}
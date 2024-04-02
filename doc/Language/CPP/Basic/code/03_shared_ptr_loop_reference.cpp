/**
 * 编译指令:
 *  g++ -o shared_ptr_loop_reference .\03_shared_ptr_loop_reference.cpp
*/

#include <iostream>
#include <memory>

#define SHARED_PTR_LOOP_REFERENCE

class Son;
class Father {
public:
    std::shared_ptr<Son> son_;

public:
    Father() {
        std::cout << "construct Father " << std::endl;
    }
    ~Father() {
        std::cout << "destruct Father " << std::endl;
    }
};

class Son {
public:
#ifdef SHARED_PTR_LOOP_REFERENCE
    std::shared_ptr<Father>  father_;
#else
    // 采用std::weak_ptr避免shared_ptr循环引用
    std::weak_ptr<Father>       father_;
#endif

public:
    Son() {
        std::cout << "construct Son " << std::endl;
    }
    ~Son() {
        std::cout << "destruct Son " << std::endl;
    }
};

int main() {
    auto son = std::make_shared<Son>();
    auto father = std::make_shared<Father>();
    son->father_ = father;
    father->son_ = son;

    std::cout << "reference count of son: " << son.use_count() << std::endl;
    std::cout << "reference count of father: " << father.use_count() << std::endl;

#ifdef SHARED_PTR_LOOP_REFERENCE
    // 手动释放成员,避免循环引用
    son->father_.reset();
    father->son_.reset();

    std::cout<< "shared ptr after reset " << std::endl << std::endl;
    std::cout << "reference count of son: " << son.use_count() << std::endl;
    std::cout << "reference count of father: " << father.use_count() << std::endl;
#endif
    return 0;
}
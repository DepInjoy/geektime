/**
 * @brief   static相关的内容
 *          1. The address of a static member function may be stored in a regular pointer to function,
 *             but not in a pointer to member function.(see https://en.cppreference.com/w/cpp/language/static)
 *          使用示例:static类型的成员函数按照非成员函数进行std::bind
 *          https://stackoverflow.com/questions/21401552/stdbind-a-static-member-function-inside-the-class
 * 
 *          2. C++ static initialization order:
 *          假设B要使用A,可以采用在B的构造中构造A，由于B先构造A后构造，因此在析构时A先析构B后析构
 *          https://stackoverflow.com/questions/335369/finding-c-static-initialization-order-problems/335746#335746
 */

#include <iostream>
#include <functional>

struct Foo {
void foo_nonstatic(int, int) {
    std::cout << "Foo::foo_nonstatic" << std::endl;
}

static int foo_static(int, int, int) {
    std::cout << "Foo::foo_static" << std::endl;
    return 42;
}
};


class A {
public:
    A() {
        std::cout << "Construct A" << std::endl;
    }

    ~A() {
        std::cout << "Destruct A" << std::endl;
    }

    void Print() {
        std::cout << "Call A Print" << std::endl;
    }

    static A& GetInstance() {
        static A a;
        return a;
    }
};

class B {
public:
    B() {
        std::cout << "Construct B" << std::endl;
        A::GetInstance();
    }

    ~B() {
        A::GetInstance().Print();
        std::cout << "Destruct B" << std::endl;
    }
};

int main(int argc, char* argv[]) {
    // 1. static类型的成员函数按照非成员函数进行std::bind
    auto f_nonstatic = std::bind((void(Foo::*)(int, int))&Foo::foo_nonstatic, Foo(), 1, 2);
    auto f_static = std::bind((int(*)(int, int, int))&Foo::foo_static, 1, 2, 3);
    f_nonstatic(66, 6);
    f_static(6, 6, 6);

    // 2. C++ static initialization order
    std::cout << "\n\nC++ static initialization order demo start:" << std::endl;
    B b;
    return 0;
}
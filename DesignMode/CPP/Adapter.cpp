/**
 * @brief   适配器模式，有对象适配器和类适配器两种模式
 *              Adapter1: 对象适配器实现示例
 *              Adapter2: 类适配器实现示例
 */

#include <iostream>

class Target {
public:
    virtual void Requet() {
        std::cout << "Common Request" << std::endl;
    }
};

class Adaptee {
public:
    void SpecialRequet() {
        std::cout << "Special Requet" << std::endl;
    }
};

// 对象适配器
class Adapter1 : public Target{
public:
    void Requet() {
        std::cout << "对象适配器实现适配功能" << std::endl;
        adaptee.SpecialRequet();
    }

private:
    Adaptee adaptee;
};

// 类适配器实现
class Adapter2 : public Target, private Adaptee {
public:
    void Requet() {
        std::cout << "类适配器实现适配功能" << std::endl;
        SpecialRequet();
    }
};

int main(int argc, char* argv[]) {
    Adapter1 adapter1;
    adapter1.Requet();

    std::cout << std::endl;

    Adapter2 adapter2;
    adapter2.Requet();
    return 0;
}
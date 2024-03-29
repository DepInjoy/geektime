/**
 * @brief   适配器模式，有对象适配器和类适配器两种模式
 *              ObjectAdapter: 对象适配器实现示例
 *              ClassAdapter: 类适配器实现示例
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
class ObjectAdapter : public Target{
public:
    void Requet() {
        std::cout << "对象适配器实现适配功能 ";
        adaptee.SpecialRequet();
    }

private:
    Adaptee adaptee;
};

// 类适配器实现
class ClassAdapter : public Target, private Adaptee {
public:
    void Requet() {
        std::cout << "类适配器实现适配功能 ";
        SpecialRequet();
    }
};

int main(int argc, char* argv[]) {
    ObjectAdapter adapter1;
    adapter1.Requet();

    std::cout << std::endl;

    ClassAdapter adapter2;
    adapter2.Requet();
    return 0;
}